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

#ifndef CONFIG_CURVE_C13318_H
#define CONFIG_CURVE_C13318_H

#include"core.h"
#include"config_field_F25519.h"

// ECP stuff

#define CURVETYPE_C13318 WEIERSTRASS
#define CURVE_A_C13318 -3
#define PAIRING_FRIENDLY_C13318 NOT_PF
#define CURVE_SECURITY_C13318 128
#define HTC_ISO_C13318 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_C13318

#if PAIRING_FRIENDLY_C13318 != NOT_PF

#define HTC_ISO_G2_C13318 0

#define USE_GLV_C13318   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_C13318 /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_C13318 /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_C13318 
#define SIGN_OF_X_C13318 

#define ATE_BITS_C13318 
#define G2_TABLE_C13318 

#endif


#if CURVE_SECURITY_C13318 == 128
#define AESKEY_C13318 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_C13318 SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_C13318 == 192
#define AESKEY_C13318 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_C13318 SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_C13318 == 256
#define AESKEY_C13318 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_C13318 SHA512  /**< Hash type */
#endif


namespace C13318_BIG = B256_56;
namespace C13318_FP = F25519;

#endif
