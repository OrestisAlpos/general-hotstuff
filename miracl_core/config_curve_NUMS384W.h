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

#ifndef CONFIG_CURVE_NUMS384W_H
#define CONFIG_CURVE_NUMS384W_H

#include"core.h"
#include"config_field_F384PM.h"

// ECP stuff

#define CURVETYPE_NUMS384W WEIERSTRASS
#define CURVE_A_NUMS384W -3
#define PAIRING_FRIENDLY_NUMS384W NOT_PF
#define CURVE_SECURITY_NUMS384W 192
#define HTC_ISO_NUMS384W 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_NUMS384W

#if PAIRING_FRIENDLY_NUMS384W != NOT_PF

#define HTC_ISO_G2_NUMS384W 0

#define USE_GLV_NUMS384W   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_NUMS384W /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_NUMS384W /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_NUMS384W 
#define SIGN_OF_X_NUMS384W 

#define ATE_BITS_NUMS384W 
#define G2_TABLE_NUMS384W 

#endif


#if CURVE_SECURITY_NUMS384W == 128
#define AESKEY_NUMS384W 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_NUMS384W SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_NUMS384W == 192
#define AESKEY_NUMS384W 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_NUMS384W SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_NUMS384W == 256
#define AESKEY_NUMS384W 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_NUMS384W SHA512  /**< Hash type */
#endif


namespace NUMS384W_BIG = B384_58;
namespace NUMS384W_FP = F384PM;

#endif
