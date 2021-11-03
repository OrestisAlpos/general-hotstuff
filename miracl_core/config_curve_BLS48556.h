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

#ifndef CONFIG_CURVE_BLS48556_H
#define CONFIG_CURVE_BLS48556_H

#include"core.h"
#include"config_field_BLS48556.h"

// ECP stuff

#define CURVETYPE_BLS48556 WEIERSTRASS
#define CURVE_A_BLS48556 0
#define PAIRING_FRIENDLY_BLS48556 BLS48_CURVE
#define CURVE_SECURITY_BLS48556 256
#define HTC_ISO_BLS48556 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_BLS48556

#if PAIRING_FRIENDLY_BLS48556 != NOT_PF

#define HTC_ISO_G2_BLS48556 0

#define USE_GLV_BLS48556   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_BLS48556 /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_BLS48556 /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_BLS48556 M_TYPE
#define SIGN_OF_X_BLS48556 POSITIVEX

#define ATE_BITS_BLS48556 32
#define G2_TABLE_BLS48556 35

#endif


#if CURVE_SECURITY_BLS48556 == 128
#define AESKEY_BLS48556 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_BLS48556 SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_BLS48556 == 192
#define AESKEY_BLS48556 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_BLS48556 SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_BLS48556 == 256
#define AESKEY_BLS48556 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_BLS48556 SHA512  /**< Hash type */
#endif


namespace BLS48556_BIG = B560_58;
namespace BLS48556_FP = BLS48556;

#endif
