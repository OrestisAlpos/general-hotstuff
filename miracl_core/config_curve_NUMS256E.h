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

#ifndef CONFIG_CURVE_NUMS256E_H
#define CONFIG_CURVE_NUMS256E_H

#include"core.h"
#include"config_field_F256PME.h"

// ECP stuff

#define CURVETYPE_NUMS256E EDWARDS
#define CURVE_A_NUMS256E 1
#define PAIRING_FRIENDLY_NUMS256E NOT_PF
#define CURVE_SECURITY_NUMS256E 128
#define HTC_ISO_NUMS256E 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_NUMS256E

#if PAIRING_FRIENDLY_NUMS256E != NOT_PF

#define HTC_ISO_G2_NUMS256E 0

#define USE_GLV_NUMS256E   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_NUMS256E /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_NUMS256E /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_NUMS256E 
#define SIGN_OF_X_NUMS256E 

#define ATE_BITS_NUMS256E 
#define G2_TABLE_NUMS256E 

#endif


#if CURVE_SECURITY_NUMS256E == 128
#define AESKEY_NUMS256E 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_NUMS256E SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_NUMS256E == 192
#define AESKEY_NUMS256E 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_NUMS256E SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_NUMS256E == 256
#define AESKEY_NUMS256E 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_NUMS256E SHA512  /**< Hash type */
#endif


namespace NUMS256E_BIG = B256_56;
namespace NUMS256E_FP = F256PME;

#endif
