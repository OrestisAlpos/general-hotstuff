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

#ifndef CONFIG_CURVE_BRAINPOOL_H
#define CONFIG_CURVE_BRAINPOOL_H

#include"core.h"
#include"config_field_BRAINPOOL.h"

// ECP stuff

#define CURVETYPE_BRAINPOOL WEIERSTRASS
#define CURVE_A_BRAINPOOL -3
#define PAIRING_FRIENDLY_BRAINPOOL NOT_PF
#define CURVE_SECURITY_BRAINPOOL 128
#define HTC_ISO_BRAINPOOL 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_BRAINPOOL

#if PAIRING_FRIENDLY_BRAINPOOL != NOT_PF

#define HTC_ISO_G2_BRAINPOOL 0

#define USE_GLV_BRAINPOOL   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_BRAINPOOL /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_BRAINPOOL /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_BRAINPOOL 
#define SIGN_OF_X_BRAINPOOL 

#define ATE_BITS_BRAINPOOL 
#define G2_TABLE_BRAINPOOL 

#endif


#if CURVE_SECURITY_BRAINPOOL == 128
#define AESKEY_BRAINPOOL 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_BRAINPOOL SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_BRAINPOOL == 192
#define AESKEY_BRAINPOOL 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_BRAINPOOL SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_BRAINPOOL == 256
#define AESKEY_BRAINPOOL 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_BRAINPOOL SHA512  /**< Hash type */
#endif


namespace BRAINPOOL_BIG = B256_56;
namespace BRAINPOOL_FP = BRAINPOOL;

#endif
