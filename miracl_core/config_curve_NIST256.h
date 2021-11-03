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

#ifndef CONFIG_CURVE_NIST256_H
#define CONFIG_CURVE_NIST256_H

#include"core.h"
#include"config_field_NIST256.h"

// ECP stuff

#define CURVETYPE_NIST256 WEIERSTRASS
#define CURVE_A_NIST256 -3
#define PAIRING_FRIENDLY_NIST256 NOT_PF
#define CURVE_SECURITY_NIST256 128
#define HTC_ISO_NIST256 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_NIST256

#if PAIRING_FRIENDLY_NIST256 != NOT_PF

#define HTC_ISO_G2_NIST256 0

#define USE_GLV_NIST256   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_NIST256 /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_NIST256 /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_NIST256 
#define SIGN_OF_X_NIST256 

#define ATE_BITS_NIST256 
#define G2_TABLE_NIST256 

#endif


#if CURVE_SECURITY_NIST256 == 128
#define AESKEY_NIST256 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_NIST256 SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_NIST256 == 192
#define AESKEY_NIST256 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_NIST256 SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_NIST256 == 256
#define AESKEY_NIST256 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_NIST256 SHA512  /**< Hash type */
#endif


namespace NIST256_BIG = B256_56;
namespace NIST256_FP = NIST256;

#endif
