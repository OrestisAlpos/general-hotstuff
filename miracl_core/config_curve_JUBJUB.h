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

#ifndef CONFIG_CURVE_JUBJUB_H
#define CONFIG_CURVE_JUBJUB_H

#include"core.h"
#include"config_field_JUBJUB.h"

// ECP stuff

#define CURVETYPE_JUBJUB EDWARDS
#define CURVE_A_JUBJUB -1
#define PAIRING_FRIENDLY_JUBJUB NOT_PF
#define CURVE_SECURITY_JUBJUB 128
#define HTC_ISO_JUBJUB 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_JUBJUB

#if PAIRING_FRIENDLY_JUBJUB != NOT_PF

#define HTC_ISO_G2_JUBJUB 0

#define USE_GLV_JUBJUB   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_JUBJUB /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_JUBJUB /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_JUBJUB 
#define SIGN_OF_X_JUBJUB 

#define ATE_BITS_JUBJUB 
#define G2_TABLE_JUBJUB 

#endif


#if CURVE_SECURITY_JUBJUB == 128
#define AESKEY_JUBJUB 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_JUBJUB SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_JUBJUB == 192
#define AESKEY_JUBJUB 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_JUBJUB SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_JUBJUB == 256
#define AESKEY_JUBJUB 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_JUBJUB SHA512  /**< Hash type */
#endif


namespace JUBJUB_BIG = B256_56;
namespace JUBJUB_FP = JUBJUB;

#endif
