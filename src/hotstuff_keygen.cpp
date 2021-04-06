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

#include <error.h>
#include <iostream>
#include <streambuf>
#include "salticidae/util.h"
#include "hotstuff/crypto.h"
#include  "hotstuff/quorums.h"

using salticidae::Config;
using hotstuff::privkey_bt;
using hotstuff::pubkey_bt;

void share(size_t t, size_t n){
    bls::init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);

    privkey_bt priv_key;
    pubkey_bt pub_key;

    bls::SecretKey sec;
    sec.init(); //generates random sk
    priv_key = new hotstuff::PrivKeyBls(sec);
	pub_key = priv_key->get_pubkey();  //get the global threshold public key
    printf("glb:%s\n", get_hex(*pub_key).c_str());

    bls::SecretKeyVec msk;			//msk contains the t coefs. of the polynomial
	sec.getMasterSecretKey(msk, t); //makes msk[0]==sec and the other entries random
	bls::SecretKeyVec secVec(n);
	bls::IdVec idVec(n);

	for (size_t i = 0; i < n; i++) {
		int id = i + 1;          //Replicas are numbered 0..n-1, but they get the values at points 1..n of the sharing polynomial. This has to be consistent with the sig. recovery in QuorumCertBls::compute()
		idVec[i] = id;
		secVec[i].set(msk, id); //evaluates the polynomial of degree msk.size()-1 with coefs. msk on point id, id>0
        priv_key = new hotstuff::PrivKeyBls(secVec[i]);
        pub_key = priv_key->get_pubkey(); //get corresponding verification key
        printf("pub:%s sec:%s\n", get_hex(*pub_key).c_str(), get_hex(*priv_key).c_str());
	}
}

inline bls::SecretKey Fr_ntl_to_mcl(const NTL::ZZ_p val){
    std::stringstream buffer;
    buffer << val;
    bls::SecretKey val_mcl;
    val_mcl.setStr(buffer.str(), 10);
    return val_mcl;
}

void shareGeneralized(size_t nreplicas, const std::string& quorumsConfFile){
    //!!!
    bls::init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
    std::string FrOrderStr;
    bls::getCurveOrder(FrOrderStr);
    NTL::ZZ_p::init(NTL::ZZ(NTL::INIT_VAL, FrOrderStr.c_str()));
    
    //init access structure
    hotstuff::quorums::JsonParser *jp = new hotstuff::quorums::ConfigJsonParser();
    hotstuff::quorums::AccessStructure accessStructure(jp);
    accessStructure.initialize(quorumsConfFile);

    //create random secret and shares
    NTL::ZZ_p secret_ntl;
    NTL::vec_ZZ_p shares_ntl;
    std::vector<hotstuff::ReplicaID> owners;
    accessStructure.msp.shareRandSecret(secret_ntl, shares_ntl, owners);

    privkey_bt priv_key;
    pubkey_bt pub_key;

    bls::SecretKey sec = Fr_ntl_to_mcl(secret_ntl);
    priv_key = new hotstuff::PrivKeyBls(sec);      //this is the global secret key, unkown to the replicas
	pub_key = priv_key->get_pubkey();
    printf("glb:%s\n", get_hex(*pub_key).c_str()); //this is the global public key

    std::unordered_map<hotstuff::ReplicaID, std::vector<bls::SecretKey>> replica_sharesOwned_map;
    for (int i = 0; i < nreplicas; i++)
            replica_sharesOwned_map.insert(std::make_pair(i, std::vector<bls::SecretKey>()));
	for (size_t j = 0; j < accessStructure.msp.m(); j++) {
		hotstuff::ReplicaID share_owner = owners[j];
        bls::SecretKey share_mcl = Fr_ntl_to_mcl(shares_ntl[j]);
        replica_sharesOwned_map[share_owner].push_back(share_mcl); //for each replica, shares are pushed back (and printed) in the order defined by the MSP
    }
    for (int i = 0; i < nreplicas; i++){
        std::vector<privkey_bt> priv_keys;
        std::vector<pubkey_bt> ver_keys;
        priv_key = new hotstuff::PrivKeyBls(replica_sharesOwned_map.at(i)); // secret key shares
        pub_key = priv_key->get_pubkey();                                   // corresponding verification keys
        printf("pub:%s sec:%s \n", get_hex(*pub_key).c_str(), get_hex(*priv_key).c_str());
	}

}

int main(int argc, char **argv) {
    Config config("hotstuff.conf");
    privkey_bt priv_key;
    auto opt_n = Config::OptValInt::create(1);
    auto opt_algo = Config::OptValStr::create("secp256k1");
    auto opt_quorums_file = Config::OptValStr::create("");
    config.add_opt("num", opt_n, Config::SET_VAL);
    config.add_opt("algo", opt_algo, Config::SET_VAL);
    config.add_opt("quorums_file", opt_quorums_file, Config::SET_VAL);
    config.parse(argc, argv);
    auto &algo = opt_algo->get();
    int n = opt_n->get();
    if (n < 1)
        error(1, 0, "n must be >0");

    if (algo == "secp256k1")
        priv_key = new hotstuff::PrivKeySecp256k1();
    else if (algo == "bls"){
            if (n == 1) error(1, 0, "n cannot be 1 in threshold BLS mode");
            #ifdef USE_BLS
            shareGeneralized(n, opt_quorums_file->get());
            #else
            share(n - n / 3, n);
            #endif
            return 0 ;
        }
    else
        error(1, 0, "algo not supported");

    while (n--)
    {
        priv_key->from_rand();
        pubkey_bt pub_key = priv_key->get_pubkey();
            printf("pub:%s sec:%s\n", get_hex(*pub_key).c_str(), get_hex(*priv_key).c_str());
    }
    return 0;
}