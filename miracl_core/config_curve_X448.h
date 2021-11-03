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

#ifndef CONFIG_CURVE_X448_H
#define CONFIG_CURVE_X448_H

#include"core.h"
#include"config_field_GOLDILOCKS.h"

// ECP stuff

#define CURVETYPE_X448 MONTGOMERY
#define CURVE_A_X448 156326
#define PAIRING_FRIENDLY_X448 NOT_PF
#define CURVE_SECURITY_X448 256
#define HTC_ISO_X448 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_X448

#if PAIRING_FRIENDLY_X448 != NOT_PF

#define HTC_ISO_G2_X448 0

#define USE_GLV_X448   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_X448 /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_X448 /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_X448 
#define SIGN_OF_X_X448 

#define ATE_BITS_X448 
#define G2_TABLE_X448 

#endif


#if CURVE_SECURITY_X448 == 128
#define AESKEY_X448 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_X448 SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_X448 == 192
#define AESKEY_X448 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_X448 SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_X448 == 256
#define AESKEY_X448 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_X448 SHA512  /**< Hash type */
#endif


namespace X448_BIG = B448_58;
namespace X448_FP = GOLDILOCKS;

#endif
