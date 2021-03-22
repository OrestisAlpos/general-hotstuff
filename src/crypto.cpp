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

#include "hotstuff/entity.h"
#include "hotstuff/crypto.h"

namespace hotstuff {

secp256k1_context_t secp256k1_default_sign_ctx = new Secp256k1Context(true);
secp256k1_context_t secp256k1_default_verify_ctx = new Secp256k1Context(false);

QuorumCertSecp256k1::QuorumCertSecp256k1(
        const ReplicaConfig &config, const uint256_t &obj_hash):
            QuorumCert(), obj_hash(obj_hash), rids(config.nreplicas) {
    rids.clear();
}
   
bool QuorumCertSecp256k1::verify(const ReplicaConfig &config) {
#ifdef USE_GENERALIZED_QUORUMS
    std::unordered_set<hotstuff::ReplicaID> reps;
    for(auto k : sigs)
        reps.insert(k.first);
    if (!config.isAuthorizedGroup(reps)) return false;
#else
    if (sigs.size() < config.nmajority) return false;
#endif
    for (size_t i = 0; i < rids.size(); i++)
        if (rids.get(i))
        {
            HOTSTUFF_LOG_DEBUG("checking cert(%d), obj_hash=%s",
                                i, get_hex10(obj_hash).c_str());
            if (!sigs[i].verify(obj_hash,
                            static_cast<const PubKeySecp256k1 &>(config.get_pubkey(i)),
                            secp256k1_default_verify_ctx))
            return false;
        }
    return true;
}

promise_t QuorumCertSecp256k1::verify(const ReplicaConfig &config, VeriPool &vpool) {
#ifdef USE_GENERALIZED_QUORUMS
    std::unordered_set<hotstuff::ReplicaID> reps;
    for(auto k : sigs)
        reps.insert(k.first);
    if (!config.isAuthorizedGroup(reps))
        return promise_t([](promise_t &pm) { pm.resolve(false); });
#else
    if (sigs.size() < config.nmajority)
        return promise_t([](promise_t &pm) { pm.resolve(false); });
#endif

    std::vector<promise_t> vpm;
    for (size_t i = 0; i < rids.size(); i++)
        if (rids.get(i))
        {
            HOTSTUFF_LOG_DEBUG("checking cert(%d), obj_hash=%s",
                                i, get_hex10(obj_hash).c_str());
            vpm.push_back(vpool.verify(new Secp256k1VeriTask(obj_hash,
                            static_cast<const PubKeySecp256k1 &>(config.get_pubkey(i)),
                            sigs[i])));
        }
    return promise::all(vpm).then([](const promise::values_t &values) {
        for (const auto &v: values)
            if (!promise::any_cast<bool>(v)) return false;
        return true;
    });
}

QuorumCertBls::QuorumCertBls(const ReplicaConfig &config, const uint256_t &obj_hash):
            QuorumCert(), obj_hash(obj_hash), rids(config.nreplicas) {rids.clear();}
   
bool QuorumCertBls::verify(const ReplicaConfig &config) {
// #ifdef USE_GENERALIZED_QUORUMS
//     std::unordered_set<hotstuff::ReplicaID> reps;
//     for(auto k : sigs)
//         reps.insert(k.first);
//     if (!config.isAuthorizedGroup(reps)) return false;
// #else
//     if (sigs.size() < config.nmajority) return false;
// #endif
//     for (size_t i = 0; i < rids.size(); i++)
//         if (rids.get(i)){
//             HOTSTUFF_LOG_DEBUG("checking cert(%d), obj_hash=%s", i, get_hex10(obj_hash).c_str());
//             if (!sigs[i].verify(obj_hash, static_cast<const PubKeyBls &>(config.get_pubkey(i)) )){
//                 HOTSTUFF_LOG_DEBUG("Non-verified signature for cert(%d), obj_hash=%s", i, get_hex10(obj_hash).c_str());
//                 return false;
//             }
                
//         }
//     return true;
    HOTSTUFF_LOG_DEBUG("checking cert on block with obj_hash=%s", get_hex10(obj_hash).c_str());
    // You will get an error if you try to verify a QC the signature of which has not yet been created
    // if (!threshSigExists)
    //     return false;
    if (!threshSig.verify(obj_hash, static_cast<const PubKeyBls &>(config.get_globalPubKey()) )){
        HOTSTUFF_LOG_DEBUG("Non-verified threshold signature for cert on block with obj_hash=%s", get_hex10(obj_hash).c_str());
        return false;
    }
    return true;
}


promise_t QuorumCertBls::verify(const ReplicaConfig &config, VeriPool &vpool) {
// #ifdef USE_GENERALIZED_QUORUMS
//     std::unordered_set<hotstuff::ReplicaID> reps;
//     for(auto k : sigs)
//         reps.insert(k.first);
//     if (!config.isAuthorizedGroup(reps))
//         return promise_t([](promise_t &pm) { pm.resolve(false); });
// #else
//     if (sigs.size() < config.nmajority)
//         return promise_t([](promise_t &pm) { pm.resolve(false); });
// #endif
//     std::vector<promise_t> vpm;
//     for (size_t i = 0; i < rids.size(); i++)
//         if (rids.get(i)){
//             HOTSTUFF_LOG_DEBUG("checking cert(%d), obj_hash=%s", i, get_hex10(obj_hash).c_str());
//             vpm.push_back(vpool.verify(new BlsVeriTask(obj_hash,
//                                                        static_cast<const PubKeyBls &>(config.get_pubkey(i)),
//                                                        sigs[i]) ));
//         }
//     return promise::all(vpm).then([](const promise::values_t &values) {
//         for (const auto &v: values)
//             if (!promise::any_cast<bool>(v)) return false;
//         return true;
//     });
    std::vector<promise_t> vpm;
    HOTSTUFF_LOG_DEBUG("checking cert on block with obj_hash=%s", get_hex10(obj_hash).c_str());
    // You will get an error if you try to verify a QC the signature of which has not yet been created
    // if (!threshSigExists) 
    //     return promise_t([](promise_t &pm) { pm.resolve(false); });
    vpm.push_back(vpool.verify(new BlsVeriTask(obj_hash, static_cast<const PubKeyBls &>(config.get_globalPubKey()), threshSig) ));
    return promise::all(vpm).then([](const promise::values_t &values) {
        for (const auto &v: values)
            if (!promise::any_cast<bool>(v)) return false;
        return true;
    });
}

// Recover threshold signature from an authorized set of replicas reps.
// This function assumes an authorized group signatures in the field sigs.
    void QuorumCertBls::compute() {
        if (sigs.size() == 0)
            HOTSTUFF_LOG_DEBUG("QuorumCertBls::compute() called on empty sigs array");
        bls::IdVec idVec(sigs.size());
        bls::SignatureVec sigVec(sigs.size());
        int i=0;
        for (auto kv: sigs){
            idVec[i] = bls::Id(uint16_t(kv.first) + 1); //Replicas are numbered 0..n-1, but they get the values at points 1..n of the sharing polynomial.
            sigVec[i] = kv.second.sig;
            HOTSTUFF_LOG_DEBUG("recover rid=%s, sig=%s", idVec[i].getStr().c_str(), sigVec[i].getStr().c_str());
            i++;
        }
        bls::Signature s;
        s.recover(sigVec, idVec);
        threshSig = SigBls(s);
        HOTSTUFF_LOG_DEBUG("Computed threshSig=%s from nreplicas=%d", get_hex(threshSig).c_str(), sigs.size());
        sigs.clear(); //This function is expected to be called only once per qc.
        rids.clear();
        threshSigExists = true;
    }
}
