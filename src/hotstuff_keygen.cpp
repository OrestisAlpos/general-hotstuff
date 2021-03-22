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
#include "salticidae/util.h"
#include "hotstuff/crypto.h"


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

int main(int argc, char **argv) {
    Config config("hotstuff.conf");
    privkey_bt priv_key;
    auto opt_n = Config::OptValInt::create(1);
    auto opt_algo = Config::OptValStr::create("secp256k1");
    config.add_opt("num", opt_n, Config::SET_VAL);
    config.add_opt("algo", opt_algo, Config::SET_VAL);
    config.parse(argc, argv);
    auto &algo = opt_algo->get();
    int n = opt_n->get();
    if (n < 1)
        error(1, 0, "n must be >0");
    
    if (algo == "secp256k1")
        priv_key = new hotstuff::PrivKeySecp256k1();
    else if (algo == "bls"){
            if (n == 1) error(1, 0, "n cannot be 1 in threshold BLS mode");
            share(n - n / 3, n);
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