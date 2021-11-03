/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
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

/**
 * @file rsa.h
 * @author Mike Scott and Kealan McCusker
 * @date 2nd June 2015
 * @brief RSA Header file for implementation of RSA protocol
 *
 * declares functions
 *
 */

#ifndef RSA_RSA3072_H
#define RSA_RSA3072_H

#include "ff_RSA3072.h"
//#include "rsa_support.h"

using namespace core;

/*** START OF USER CONFIGURABLE SECTION -  ***/

#define HASH_TYPE_RSA_RSA3072 SHA256 /**< Chosen Hash algorithm */

/*** END OF USER CONFIGURABLE SECTION ***/

#define RFS_RSA3072 MODBYTES_B384_56*FFLEN_RSA3072 /**< RSA Public Key Size in bytes */


namespace RSA3072 {

/**
    @brief Integer Factorisation Public Key
*/

typedef struct
{
    sign32 e;     /**< RSA exponent (typically 65537) */
    B384_56::BIG n[FFLEN_RSA3072]; /**< An array of BIGs to store public key */
} rsa_public_key;

/**
    @brief Integer Factorisation Private Key
*/

typedef struct
{
    B384_56::BIG p[FFLEN_RSA3072 / 2]; /**< secret prime p  */
    B384_56::BIG q[FFLEN_RSA3072 / 2]; /**< secret prime q  */
    B384_56::BIG dp[FFLEN_RSA3072 / 2]; /**< decrypting exponent mod (p-1)  */
    B384_56::BIG dq[FFLEN_RSA3072 / 2]; /**< decrypting exponent mod (q-1)  */
    B384_56::BIG c[FFLEN_RSA3072 / 2]; /**< 1/p mod q */
} rsa_private_key;


/** @brief RSA Private KEy from OpenSSL 
 *
    @param P Input prime number
    @param Q Input prime number
    @param DP Input 1/e mod p-1
    @param DQ Input 1/e mod q-1
    @param C Input 1/p mod q
    @param PRIV the output RSA private key
 */
extern void RSA_PRIVATE_KEY_FROM_OPENSSL(octet *P,octet* Q,octet *DP,octet *DQ,octet *C,rsa_private_key *PRIV);

/** @brief RSA Key Pair from OpenSSL 
 *
    @param e the encryption exponent
    @param P Input prime number
    @param Q Input prime number
    @param DP Input 1/e mod p-1
    @param DQ Input 1/e mod q-1
    @param C Input 1/p mod q
    @param PRIV the output RSA private key
    @param PUB the output RSA public key
 */
extern void RSA_KEY_PAIR_FROM_OPENSSL(sign32 e,octet *P,octet* Q,octet *DP,octet *DQ,octet *C,rsa_private_key *PRIV,rsa_public_key *PUB);

/** @brief RSA Key Pair Generator
 *
    @param R is a pointer to a cryptographically secure random number generator
    @param e the encryption exponent
    @param PRIV the output RSA private key
    @param PUB the output RSA public key
    @param P Input prime number. Used when R is equal to NULL for testing
    @param Q Input prime number. Used when R is equal to NULL for testing
 */
extern void RSA_KEY_PAIR(csprng *R, sign32 e, rsa_private_key* PRIV, rsa_public_key* PUB, octet *P, octet* Q);

/** @brief RSA encryption of suitably padded plaintext
 *
    @param PUB the input RSA public key
    @param F is input padded message
    @param G is the output ciphertext
 */
extern void RSA_ENCRYPT(rsa_public_key* PUB, octet *F, octet *G);
/** @brief RSA decryption of ciphertext
 *
    @param PRIV the input RSA private key
    @param G is the input ciphertext
    @param F is output plaintext (requires unpadding)

 */
extern void RSA_DECRYPT(rsa_private_key* PRIV, octet *G, octet *F);
/** @brief Destroy an RSA private Key
 *
    @param PRIV the input RSA private key. Destroyed on output.
 */
extern void RSA_PRIVATE_KEY_KILL(rsa_private_key *PRIV);
/** @brief Populates an RSA public key from an octet string
 *
    Creates RSA public key from big-endian base 256 form.
    @param x FF instance to be created from an octet string
    @param S input octet string
 */
extern void RSA_fromOctet(B384_56::BIG *x, octet *S);
}


#endif
