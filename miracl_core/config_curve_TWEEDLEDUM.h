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

#ifndef CONFIG_CURVE_TWEEDLEDUM_H
#define CONFIG_CURVE_TWEEDLEDUM_H

#include"core.h"
#include"config_field_TWEEDLEDUM.h"

// ECP stuff

#define CURVETYPE_TWEEDLEDUM WEIERSTRASS
#define CURVE_A_TWEEDLEDUM 0
#define PAIRING_FRIENDLY_TWEEDLEDUM NOT_PF
#define CURVE_SECURITY_TWEEDLEDUM 128
#define HTC_ISO_TWEEDLEDUM 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_TWEEDLEDUM

#if PAIRING_FRIENDLY_TWEEDLEDUM != NOT_PF

#define HTC_ISO_G2_TWEEDLEDUM 0

#define USE_GLV_TWEEDLEDUM   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_TWEEDLEDUM /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_TWEEDLEDUM /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_TWEEDLEDUM 
#define SIGN_OF_X_TWEEDLEDUM 

#define ATE_BITS_TWEEDLEDUM 
#define G2_TABLE_TWEEDLEDUM 

#endif


#if CURVE_SECURITY_TWEEDLEDUM == 128
#define AESKEY_TWEEDLEDUM 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_TWEEDLEDUM SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_TWEEDLEDUM == 192
#define AESKEY_TWEEDLEDUM 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_TWEEDLEDUM SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_TWEEDLEDUM == 256
#define AESKEY_TWEEDLEDUM 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_TWEEDLEDUM SHA512  /**< Hash type */
#endif


namespace TWEEDLEDUM_BIG = B256_56;
namespace TWEEDLEDUM_FP = TWEEDLEDUM;

#endif
