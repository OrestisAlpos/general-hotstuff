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

#ifndef CONFIG_CURVE_BN254CX_H
#define CONFIG_CURVE_BN254CX_H

#include"core.h"
#include"config_field_BN254CX.h"

// ECP stuff

#define CURVETYPE_BN254CX WEIERSTRASS
#define CURVE_A_BN254CX 0
#define PAIRING_FRIENDLY_BN254CX BN_CURVE
#define CURVE_SECURITY_BN254CX 128
#define HTC_ISO_BN254CX 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_BN254CX

#if PAIRING_FRIENDLY_BN254CX != NOT_PF

#define HTC_ISO_G2_BN254CX 0

#define USE_GLV_BN254CX   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_BN254CX /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_BN254CX /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_BN254CX D_TYPE
#define SIGN_OF_X_BN254CX NEGATIVEX

#define ATE_BITS_BN254CX 66
#define G2_TABLE_BN254CX 76

#endif


#if CURVE_SECURITY_BN254CX == 128
#define AESKEY_BN254CX 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_BN254CX SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_BN254CX == 192
#define AESKEY_BN254CX 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_BN254CX SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_BN254CX == 256
#define AESKEY_BN254CX 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_BN254CX SHA512  /**< Hash type */
#endif


namespace BN254CX_BIG = B256_56;
namespace BN254CX_FP = BN254CX;

#endif
