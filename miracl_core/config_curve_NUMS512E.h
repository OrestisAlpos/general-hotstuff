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

#ifndef CONFIG_CURVE_NUMS512E_H
#define CONFIG_CURVE_NUMS512E_H

#include"core.h"
#include"config_field_F512PM.h"

// ECP stuff

#define CURVETYPE_NUMS512E EDWARDS
#define CURVE_A_NUMS512E 1
#define PAIRING_FRIENDLY_NUMS512E NOT_PF
#define CURVE_SECURITY_NUMS512E 256
#define HTC_ISO_NUMS512E 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_NUMS512E

#if PAIRING_FRIENDLY_NUMS512E != NOT_PF

#define HTC_ISO_G2_NUMS512E 0

#define USE_GLV_NUMS512E   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_NUMS512E /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_NUMS512E /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_NUMS512E 
#define SIGN_OF_X_NUMS512E 

#define ATE_BITS_NUMS512E 
#define G2_TABLE_NUMS512E 

#endif


#if CURVE_SECURITY_NUMS512E == 128
#define AESKEY_NUMS512E 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_NUMS512E SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_NUMS512E == 192
#define AESKEY_NUMS512E 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_NUMS512E SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_NUMS512E == 256
#define AESKEY_NUMS512E 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_NUMS512E SHA512  /**< Hash type */
#endif


namespace NUMS512E_BIG = B512_60;
namespace NUMS512E_FP = F512PM;

#endif
