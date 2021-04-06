/**
 * Copyright 2018 VMware
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _HOTSTUFF_CRYPTO_H
#define _HOTSTUFF_CRYPTO_H

#include <openssl/rand.h>

#include <bls/bls384_256.h>
#include <bls/bls.hpp>

#include "secp256k1.h"
#include "salticidae/crypto.h"
#include "hotstuff/type.h"
#include "hotstuff/task.h"
#include "hotstuff/util.h"

#include <sstream>

namespace hotstuff {

using salticidae::SHA256;

class PubKey: public Serializable, Cloneable {
    public:
    virtual ~PubKey() = default;
    virtual PubKey *clone() override = 0;
    
    operator std::string () const {
        std::string s = to_hex();
        return s;
    }
};

using pubkey_bt = BoxObj<PubKey>;

class PrivKey: public Serializable {
    public:
    virtual ~PrivKey() = default;
    virtual pubkey_bt get_pubkey() const = 0;
    virtual void from_rand(size_t count = 1) = 0;
};

using privkey_bt = BoxObj<PrivKey>;

class PartCert: public Serializable, public Cloneable {
    public:
    virtual ~PartCert() = default;
    virtual promise_t verify(const PubKey &pubkey, VeriPool &vpool) = 0;
    virtual bool verify(const PubKey &pubkey) = 0;
    virtual const uint256_t &get_obj_hash() const = 0;
    virtual PartCert *clone() override = 0;
};

class ReplicaConfig;

class QuorumCert: public Serializable, public Cloneable {
    public:
    virtual ~QuorumCert() = default;
    virtual void add_part(ReplicaID replica, const PartCert &pc) = 0;
    virtual void compute(const ReplicaConfig &config) = 0;
    virtual promise_t verify(const ReplicaConfig &config, VeriPool &vpool) = 0;
    virtual bool verify(const ReplicaConfig &config) = 0;
    virtual const uint256_t &get_obj_hash() const = 0;
    virtual QuorumCert *clone() override = 0;
};

using part_cert_bt = BoxObj<PartCert>;
using quorum_cert_bt = BoxObj<QuorumCert>;

class PubKeyDummy: public PubKey {
    PubKeyDummy *clone() override { return new PubKeyDummy(*this); }
    void serialize(DataStream &) const override {}
    void unserialize(DataStream &) override {}
};

class PrivKeyDummy: public PrivKey {
    pubkey_bt get_pubkey() const override { return new PubKeyDummy(); }
    void serialize(DataStream &) const override {}
    void unserialize(DataStream &) override {}
    void from_rand(size_t count = 1) override {}
};

class PartCertDummy: public PartCert {
    uint256_t obj_hash;
    public:
    PartCertDummy() {}
    PartCertDummy(const uint256_t &obj_hash):
        obj_hash(obj_hash) {}

    void serialize(DataStream &s) const override {
        s << (uint32_t)0 << obj_hash;
    }

    void unserialize(DataStream &s) override {
        uint32_t tmp;
        s >> tmp >> obj_hash;
    }

    PartCert *clone() override {
        return new PartCertDummy(obj_hash);
    }

    bool verify(const PubKey &) override { return true; }
    promise_t verify(const PubKey &, VeriPool &) override {
        return promise_t([](promise_t &pm){ pm.resolve(true); });
    }

    const uint256_t &get_obj_hash() const override { return obj_hash; }
};

class QuorumCertDummy: public QuorumCert {
    uint256_t obj_hash;
    public:
    QuorumCertDummy() {}
    QuorumCertDummy(const ReplicaConfig &, const uint256_t &obj_hash):
        obj_hash(obj_hash) {}

    void serialize(DataStream &s) const override {
        s << (uint32_t)1 << obj_hash;
    }

    void unserialize(DataStream &s) override {
        uint32_t tmp;
        s >> tmp >> obj_hash;
    }

    QuorumCert *clone() override {
        return new QuorumCertDummy(*this);
    }

    void add_part(ReplicaID, const PartCert &) override {}
    void compute(const ReplicaConfig &config) override {}
    bool verify(const ReplicaConfig &) override { return true; }
    promise_t verify(const ReplicaConfig &, VeriPool &) override {
        return promise_t([](promise_t &pm) { pm.resolve(true); });
    }

    const uint256_t &get_obj_hash() const override { return obj_hash; }
};


class Secp256k1Context {
    secp256k1_context *ctx;
    friend class PubKeySecp256k1;
    friend class SigSecp256k1;
    public:
    Secp256k1Context(bool sign = false):
        ctx(secp256k1_context_create(
            sign ? SECP256K1_CONTEXT_SIGN :
                    SECP256K1_CONTEXT_VERIFY)) {}

    Secp256k1Context(const Secp256k1Context &) = delete;

    Secp256k1Context(Secp256k1Context &&other): ctx(other.ctx) {
        other.ctx = nullptr;
    }

    ~Secp256k1Context() {
        if (ctx) secp256k1_context_destroy(ctx);
    }
};

using secp256k1_context_t = ArcObj<Secp256k1Context>;

extern secp256k1_context_t secp256k1_default_sign_ctx;
extern secp256k1_context_t secp256k1_default_verify_ctx;

class PrivKeySecp256k1;

class PubKeySecp256k1: public PubKey {
    static const auto _olen = 33;
    friend class SigSecp256k1;
    secp256k1_pubkey data;
    secp256k1_context_t ctx;

    public:
    PubKeySecp256k1(const secp256k1_context_t &ctx =
                            secp256k1_default_sign_ctx):
        PubKey(), ctx(ctx) {}
    
    PubKeySecp256k1(const bytearray_t &raw_bytes,
                    const secp256k1_context_t &ctx = secp256k1_default_sign_ctx):
        PubKeySecp256k1(ctx) { from_bytes(raw_bytes); }

    inline PubKeySecp256k1(const PrivKeySecp256k1 &priv_key,
                            const secp256k1_context_t &ctx =
                                    secp256k1_default_sign_ctx);

    void serialize(DataStream &s) const override {
        static uint8_t output[_olen];
        size_t olen = _olen;
        (void)secp256k1_ec_pubkey_serialize(
                ctx->ctx, (unsigned char *)output,
                &olen, &data, SECP256K1_EC_COMPRESSED);
        s.put_data(output, output + _olen);
    }

    void unserialize(DataStream &s) override {
        static const auto _exc = std::invalid_argument("ill-formed public key");
        try {
            if (!secp256k1_ec_pubkey_parse(
                    ctx->ctx, &data, s.get_data_inplace(_olen), _olen))
                throw _exc;
        } catch (std::ios_base::failure &) {
            throw _exc;
        }
    }

    PubKeySecp256k1 *clone() override {
        return new PubKeySecp256k1(*this);
    }

};

class PrivKeyBls;
 
class PubKeyBls: public PubKey {
    friend class SigBls;
    std::vector<bls::PublicKey> vks;
    static const size_t blsPubKeySize = 96;

    public:
    PubKeyBls(): PubKey(){}

    PubKeyBls(const bytearray_t &raw_bytes):
        PubKeyBls() { 
            DataStream ds(raw_bytes);
            unserialize(ds);
        }

    inline PubKeyBls(const PrivKeyBls &priv_key);

    PubKeyBls(const std::vector<bls::PublicKey> vks):
        PubKey(), vks(vks) {}

    void serialize(DataStream &s) const override {
        s << vks.size();
        for (size_t i = 0; i < vks.size(); i++){
            // Serialize each vks.at(i)
            DataStream ds;
            ds.load_hex(vks.at(i).serializeToHexStr()); //Expected to write blsPubKeySize bytes to ds.
            s.put_data(ds.data(), ds.data() + blsPubKeySize);
        }
    }

    void unserialize(DataStream &s) override {
        vks.clear();
        size_t size;
        s >> size;
        for (size_t i = 0; i < size; i++){
            //Unserialize a pk size times
            bls::PublicKey pk;
            uint8_t data[blsPubKeySize];
            memmove(data, s.get_data_inplace(blsPubKeySize), blsPubKeySize);
            auto pkHexStr = DataStream(data, data + blsPubKeySize).get_hex();
            pk.deserializeHexStr(pkHexStr);
            vks.push_back(pk);
        }
    }

    PubKeyBls *clone() override {
        return new PubKeyBls(*this);
    }

};

class PrivKeySecp256k1: public PrivKey {
    static const auto nbytes = 32;
    friend class PubKeySecp256k1;
    friend class SigSecp256k1;
    uint8_t data[nbytes];
    secp256k1_context_t ctx;

    public:
    PrivKeySecp256k1(const secp256k1_context_t &ctx =
                            secp256k1_default_sign_ctx):
        PrivKey(), ctx(ctx) {}

    PrivKeySecp256k1(const bytearray_t &raw_bytes,
                     const secp256k1_context_t &ctx =
                            secp256k1_default_sign_ctx):
        PrivKeySecp256k1(ctx) { from_bytes(raw_bytes); }

    void serialize(DataStream &s) const override {
        s.put_data(data, data + nbytes);
    }

    void unserialize(DataStream &s) override {
        static const auto _exc = std::invalid_argument("ill-formed private key");
        try {
            memmove(data, s.get_data_inplace(nbytes), nbytes);
        } catch (std::ios_base::failure &) {
            throw _exc;
        }
    }

    void from_rand(size_t count = 1) override {
        if (!RAND_bytes(data, nbytes))
            throw std::runtime_error("cannot get rand bytes from openssl");
    }

    inline pubkey_bt get_pubkey() const override;
};

pubkey_bt PrivKeySecp256k1::get_pubkey() const {
    return new PubKeySecp256k1(*this, ctx);
}

PubKeySecp256k1::PubKeySecp256k1(
        const PrivKeySecp256k1 &priv_key,
        const secp256k1_context_t &ctx): PubKey(), ctx(ctx) {
    if (!secp256k1_ec_pubkey_create(ctx->ctx, &data, priv_key.data))
        throw std::invalid_argument("invalid secp256k1 private key");
}


class PrivKeyBls: public PrivKey {
    friend class PubKeyBls;
    friend class SigBls;
    std::vector<bls::SecretKey> sks;
    static const size_t blsPrivKeySize = 32;

    public:
    PrivKeyBls(): PrivKey(){}

    PrivKeyBls(const bytearray_t &raw_bytes):
        PrivKeyBls() { 
            DataStream ds(raw_bytes);
            unserialize(ds);
        }

    PrivKeyBls(const std::vector<bls::SecretKey> sks): 
        PrivKey(), sks(sks) {}

    PrivKeyBls(const bls::SecretKey sk): 
        PrivKey() {sks.push_back(sk);}

    void serialize(DataStream &s) const override {
        s << sks.size();
        for (size_t i = 0; i < sks.size(); i++){
            // Serialize each sks.at(i)
            DataStream ds;
            ds.load_hex(sks.at(i).serializeToHexStr()); //Expected to write blsPrivKeySize bytes to ds.
            s.put_data(ds.data(), ds.data() + blsPrivKeySize);   
        }
    }

    void unserialize(DataStream &s) override {
        sks.clear();
        size_t size;
        s >> size;
        for (size_t i = 0; i < size; i++){
            //Unserialize an sk size times
            bls::SecretKey sk;
            uint8_t data[blsPrivKeySize];
            memmove(data, s.get_data_inplace(blsPrivKeySize), blsPrivKeySize);
            auto skHexStr = DataStream(data, data + blsPrivKeySize).get_hex();
            sk.deserializeHexStr(skHexStr);
            sks.push_back(sk);
        }        
    }

    void from_rand(size_t count =  1) override {
        sks.clear();
        for (size_t i = 0; i < count; i++){
            bls::SecretKey sk;
            sk.init();
            sks.push_back(sk);
        }
    }

    inline pubkey_bt get_pubkey() const override{
        return new PubKeyBls(*this);
    }
};

PubKeyBls::PubKeyBls(const PrivKeyBls &priv_key):
    PubKey() {
        for (auto sk: priv_key.sks){
            bls::PublicKey pk;
            sk.getPublicKey(pk);
            vks.push_back(pk);
        }
    }


class SigSecp256k1: public Serializable {
    secp256k1_ecdsa_signature data;
    secp256k1_context_t ctx;

    static void check_msg_length(const bytearray_t &msg) {
        if (msg.size() != 32)
            throw std::invalid_argument("the message should be 32-bytes");
    }

    public:
    SigSecp256k1(const secp256k1_context_t &ctx = secp256k1_default_sign_ctx):
        Serializable(), ctx(ctx) {}
    SigSecp256k1(const uint256_t &digest,
                 const PrivKeySecp256k1 &priv_key,
                 secp256k1_context_t &ctx = secp256k1_default_sign_ctx):
        Serializable(), ctx(ctx) {
        sign(digest, priv_key);
    }

    void serialize(DataStream &s) const override {
        static uint8_t output[64];
        (void)secp256k1_ecdsa_signature_serialize_compact(
            ctx->ctx, (unsigned char *)output,
            &data);
        s.put_data(output, output + 64);
    }

    void unserialize(DataStream &s) override {
        static const auto _exc = std::invalid_argument("ill-formed signature");
        try {
            if (!secp256k1_ecdsa_signature_parse_compact(
                    ctx->ctx, &data, s.get_data_inplace(64)))
                throw _exc;
        } catch (std::ios_base::failure &) {
            throw _exc;
        }
    } 

    void sign(const bytearray_t &msg, const PrivKeySecp256k1 &priv_key) {
        check_msg_length(msg);
        if (!secp256k1_ecdsa_sign(
                ctx->ctx, &data,
                (unsigned char *)&*msg.begin(),
                (unsigned char *)priv_key.data,
                NULL, // default nonce function
                NULL))
            throw std::invalid_argument("failed to create secp256k1 signature");
    }

    bool verify(const bytearray_t &msg, const PubKeySecp256k1 &pub_key,
                const secp256k1_context_t &_ctx) const {
        check_msg_length(msg);
        return secp256k1_ecdsa_verify(
                _ctx->ctx, &data,
                (unsigned char *)&*msg.begin(),
                &pub_key.data) == 1;
    }

    bool verify(const bytearray_t &msg, const PubKeySecp256k1 &pub_key) {
        return verify(msg, pub_key, ctx);
    }
};

class Secp256k1VeriTask: public VeriTask {
    uint256_t msg;
    PubKeySecp256k1 pubkey;
    SigSecp256k1 sig;
    public:
    Secp256k1VeriTask(const uint256_t &msg,
                        const PubKeySecp256k1 &pubkey,
                        const SigSecp256k1 &sig):
        msg(msg), pubkey(pubkey), sig(sig) {}
    virtual ~Secp256k1VeriTask() = default;

    bool verify() override {
        return sig.verify(msg, pubkey, secp256k1_default_verify_ctx);
    }
};

class SigBls: public Serializable {
    friend class QuorumCertBls;

    std::vector<bls::Signature> sigs;
    static const size_t blsSigSize = 48;

    public:
    SigBls():
        Serializable() {}
        
    SigBls(const bytearray_t &msg, const PrivKeyBls &priv_key):
        Serializable() {sign(msg, priv_key);}

    SigBls(const std::vector<bls::Signature> sigs):
        Serializable(), sigs(sigs) {}

    SigBls(const bls::Signature sig):
        Serializable() {sigs.push_back(sig);}

    void serialize(DataStream &s) const override {
        s << sigs.size();
        for (size_t i = 0; i < sigs.size(); i++){
            // Serialize each sigs.at(i)
            DataStream ds;
            ds.load_hex(sigs.at(i).serializeToHexStr()); //Expected to write blsSigSize bytes to ds.
            s.put_data(ds.data(), ds.data() + blsSigSize);
        }
    }

    void unserialize(DataStream &s) override {
        sigs.clear();
        size_t size;
        s >> size;
        for (size_t i = 0; i < size; i++){
            //Unserialize a sig size times
            bls::Signature sig;
            uint8_t data[blsSigSize];
            memmove(data, s.get_data_inplace(blsSigSize), blsSigSize);
            auto sigHexStr = DataStream(data, data + blsSigSize).get_hex();
            sig.deserializeHexStr(sigHexStr);    
            sigs.push_back(sig);
        }        
    }

    void sign(const bytearray_t &msg, const PrivKeyBls &priv_key) {
        for (bls::SecretKey sk: priv_key.sks) {
            bls::Signature sig;
            sk.sign(sig, std::string(msg.begin(), msg.end()));
            sigs.push_back(sig);
        }
    }

    bool verify(const bytearray_t &msg, const PubKeyBls &pub_key) const {
        bool isValid = true;
        if (sigs.size() != pub_key.vks.size())
            isValid = false;
        for (size_t i = 0; i < sigs.size(); i++)
            if (!(sigs[i].verify(pub_key.vks[i], std::string(msg.begin(), msg.end()))))
                isValid = false;;
        if (!isValid)
            HOTSTUFF_LOG_DEBUG("Non-verified signature for msg=%s", std::string(msg.begin(), msg.end()).c_str());        
        return isValid;
    }

};

class BlsVeriTask: public VeriTask {
    uint256_t msg;
    PubKeyBls pubkey;
    SigBls sig;
    public:
    BlsVeriTask(const uint256_t &msg,
                const PubKeyBls &pubkey,
                const SigBls &sig):
        msg(msg), pubkey(pubkey), sig(sig) {}
    virtual ~BlsVeriTask() = default;

    bool verify() override {
        return sig.verify(msg, pubkey);
    }
};

class PartCertSecp256k1: public SigSecp256k1, public PartCert {
    uint256_t obj_hash;

    public:
    PartCertSecp256k1() = default;
    PartCertSecp256k1(const PrivKeySecp256k1 &priv_key, const uint256_t &obj_hash):
        SigSecp256k1(obj_hash, priv_key),
        PartCert(),
        obj_hash(obj_hash) {}

    bool verify(const PubKey &pubkey) override {
        return SigSecp256k1::verify(obj_hash,
                                    static_cast<const PubKeySecp256k1 &>(pubkey),
                                    secp256k1_default_verify_ctx);
    }

    promise_t verify(const PubKey &pubkey, VeriPool &vpool) override {
        return vpool.verify(new Secp256k1VeriTask(obj_hash,
                static_cast<const PubKeySecp256k1 &>(pubkey),
                static_cast<const SigSecp256k1 &>(*this)));
    }

    const uint256_t &get_obj_hash() const override { return obj_hash; }

    PartCertSecp256k1 *clone() override {
        return new PartCertSecp256k1(*this);
    }

    void serialize(DataStream &s) const override {
        s << obj_hash;
        this->SigSecp256k1::serialize(s);
    }

    void unserialize(DataStream &s) override {
        s >> obj_hash;
        this->SigSecp256k1::unserialize(s);
    }
};

class PartCertBls: public SigBls, public PartCert {
    uint256_t obj_hash;

    public:
    PartCertBls() = default;
    PartCertBls(const PrivKeyBls &priv_key, const uint256_t &obj_hash):
        SigBls(obj_hash, priv_key),
        PartCert(),
        obj_hash(obj_hash) {}

    bool verify(const PubKey &pubkey) override {
        return SigBls::verify(obj_hash, static_cast<const PubKeyBls &>(pubkey));
    }

    promise_t verify(const PubKey &pubkey, VeriPool &vpool) override {
        return vpool.verify(new BlsVeriTask(obj_hash,
                static_cast<const PubKeyBls &>(pubkey),
                static_cast<const SigBls &>(*this)));
    }

    const uint256_t &get_obj_hash() const override { return obj_hash; }

    PartCertBls *clone() override {
        return new PartCertBls(*this);
    }

    void serialize(DataStream &s) const override {
        HOTSTUFF_LOG_DEBUG("Serializing PartCertBls with obj_hash= %.10s", get_hex(obj_hash).c_str());
        s << obj_hash;
        this->SigBls::serialize(s);
    }

    void unserialize(DataStream &s) override {
        HOTSTUFF_LOG_DEBUG("Unserializing a PartCertBls");
        s >> obj_hash;
        this->SigBls::unserialize(s);
    }
};

class QuorumCertSecp256k1: public QuorumCert {
    uint256_t obj_hash;
    salticidae::Bits rids;
    std::unordered_map<ReplicaID, SigSecp256k1> sigs;

    public:
    QuorumCertSecp256k1() = default;
    QuorumCertSecp256k1(const ReplicaConfig &config, const uint256_t &obj_hash);

    void add_part(ReplicaID rid, const PartCert &pc) override {
        if (pc.get_obj_hash() != obj_hash)
            throw std::invalid_argument("PartCert does match the block hash");
        sigs.insert(std::make_pair(
            rid, static_cast<const PartCertSecp256k1 &>(pc)));
        rids.set(rid);
    }

    void compute(const ReplicaConfig &config) override {}

    bool verify(const ReplicaConfig &config) override;
    promise_t verify(const ReplicaConfig &config, VeriPool &vpool) override;

    const uint256_t &get_obj_hash() const override { return obj_hash; }

    QuorumCertSecp256k1 *clone() override {
        return new QuorumCertSecp256k1(*this);
    }

    void serialize(DataStream &s) const override {
        s << obj_hash << rids;
        for (size_t i = 0; i < rids.size(); i++)
            if (rids.get(i)) s << sigs.at(i);
    }

    void unserialize(DataStream &s) override {
        s >> obj_hash >> rids;
        for (size_t i = 0; i < rids.size(); i++)
            if (rids.get(i)) s >> sigs[i];
    }
};

class QuorumCertBls: public QuorumCert {
    uint256_t obj_hash;
    salticidae::Bits rids;
    bool threshSigExists = false;
    std::unordered_map<ReplicaID, SigBls> sigs; //temporarily keep sigs here. Field is not serialized.
    SigBls threshSig;

    public:
    QuorumCertBls() = default;
    QuorumCertBls(const ReplicaConfig &config, const uint256_t &obj_hash);

    void add_part(ReplicaID rid, const PartCert &pc) override {
        if (pc.get_obj_hash() != obj_hash)
            throw std::invalid_argument("PartCert does match the block hash");
        sigs.insert(std::make_pair(rid, static_cast<const PartCertBls &>(pc)));
        rids.set(rid);
    }

    bool verify(const ReplicaConfig &config) override;
    promise_t verify(const ReplicaConfig &config, VeriPool &vpool) override;
    void compute(const ReplicaConfig &config) override;

    const uint256_t &get_obj_hash() const override { return obj_hash; }

    QuorumCertBls *clone() override {
        return new QuorumCertBls(*this);
    }

    void serialize(DataStream &s) const override { //Serialize only the threshold signature, not the partial sigs.
        HOTSTUFF_LOG_DEBUG("Serializing QuorumCertBls with obj_hash= %.10s and signature= %s", get_hex(obj_hash).c_str(), get_hex(threshSig).c_str());
        s << obj_hash;
        s << threshSigExists;
        if (threshSigExists) threshSig.serialize(s);        
    }

    void unserialize(DataStream &s) override {
        HOTSTUFF_LOG_DEBUG("Unserializing a QuorumCertBls");
        s >> obj_hash;
        s >> threshSigExists;
        if (threshSigExists) threshSig.unserialize(s);
    }

    void recoverGenThreshSig(const ReplicaConfig& config);
    void recoverThreshSig();
};

}

#endif
