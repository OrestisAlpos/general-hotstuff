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

#ifndef CONFIG_CURVE_FP512BN_H
#define CONFIG_CURVE_FP512BN_H

#include"core.h"
#include"config_field_FP512BN.h"

// ECP stuff

#define CURVETYPE_FP512BN WEIERSTRASS
#define CURVE_A_FP512BN 0
#define PAIRING_FRIENDLY_FP512BN BN_CURVE
#define CURVE_SECURITY_FP512BN 128
#define HTC_ISO_FP512BN 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_FP512BN

#if PAIRING_FRIENDLY_FP512BN != NOT_PF

#define HTC_ISO_G2_FP512BN 0

#define USE_GLV_FP512BN   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_FP512BN /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_FP512BN /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_FP512BN M_TYPE
#define SIGN_OF_X_FP512BN POSITIVEX

#define ATE_BITS_FP512BN 130
#define G2_TABLE_FP512BN 172

#endif


#if CURVE_SECURITY_FP512BN == 128
#define AESKEY_FP512BN 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_FP512BN SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_FP512BN == 192
#define AESKEY_FP512BN 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_FP512BN SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_FP512BN == 256
#define AESKEY_FP512BN 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_FP512BN SHA512  /**< Hash type */
#endif


namespace FP512BN_BIG = B512_60;
namespace FP512BN_FP = FP512BN;

#endif
