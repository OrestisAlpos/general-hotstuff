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

#ifndef CONFIG_CURVE_HIFIVE_H
#define CONFIG_CURVE_HIFIVE_H

#include"core.h"
#include"config_field_HIFIVE.h"

// ECP stuff

#define CURVETYPE_HIFIVE EDWARDS
#define CURVE_A_HIFIVE 1
#define PAIRING_FRIENDLY_HIFIVE NOT_PF
#define CURVE_SECURITY_HIFIVE 192
#define HTC_ISO_HIFIVE 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_HIFIVE

#if PAIRING_FRIENDLY_HIFIVE != NOT_PF

#define HTC_ISO_G2_HIFIVE 0

#define USE_GLV_HIFIVE   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_HIFIVE /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_HIFIVE /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_HIFIVE 
#define SIGN_OF_X_HIFIVE 

#define ATE_BITS_HIFIVE 
#define G2_TABLE_HIFIVE 

#endif


#if CURVE_SECURITY_HIFIVE == 128
#define AESKEY_HIFIVE 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_HIFIVE SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_HIFIVE == 192
#define AESKEY_HIFIVE 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_HIFIVE SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_HIFIVE == 256
#define AESKEY_HIFIVE 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_HIFIVE SHA512  /**< Hash type */
#endif


namespace HIFIVE_BIG = B336_60;
namespace HIFIVE_FP = HIFIVE;

#endif
