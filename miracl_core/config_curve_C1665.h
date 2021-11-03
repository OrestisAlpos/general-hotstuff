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

#ifndef CONFIG_CURVE_C1665_H
#define CONFIG_CURVE_C1665_H

#include"core.h"
#include"config_field_C1665.h"

// ECP stuff

#define CURVETYPE_C1665 EDWARDS
#define CURVE_A_C1665 1
#define PAIRING_FRIENDLY_C1665 NOT_PF
#define CURVE_SECURITY_C1665 128
#define HTC_ISO_C1665 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_C1665

#if PAIRING_FRIENDLY_C1665 != NOT_PF

#define HTC_ISO_G2_C1665 0

#define USE_GLV_C1665   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_C1665 /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_C1665 /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_C1665 
#define SIGN_OF_X_C1665 

#define ATE_BITS_C1665 
#define G2_TABLE_C1665 

#endif


#if CURVE_SECURITY_C1665 == 128
#define AESKEY_C1665 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_C1665 SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_C1665 == 192
#define AESKEY_C1665 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_C1665 SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_C1665 == 256
#define AESKEY_C1665 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_C1665 SHA512  /**< Hash type */
#endif


namespace C1665_BIG = B168_60;
namespace C1665_FP = C1665;

#endif
