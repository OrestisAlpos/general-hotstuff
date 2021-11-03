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

#ifndef CONFIG_CURVE_BLS24479_H
#define CONFIG_CURVE_BLS24479_H

#include"core.h"
#include"config_field_BLS24479.h"

// ECP stuff

#define CURVETYPE_BLS24479 WEIERSTRASS
#define CURVE_A_BLS24479 0
#define PAIRING_FRIENDLY_BLS24479 BLS24_CURVE
#define CURVE_SECURITY_BLS24479 192
#define HTC_ISO_BLS24479 0

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
// #define ALLOW_ALT_COMPRESS_BLS24479

#if PAIRING_FRIENDLY_BLS24479 != NOT_PF

#define HTC_ISO_G2_BLS24479 0

#define USE_GLV_BLS24479   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_BLS24479 /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_BLS24479 /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_BLS24479 M_TYPE
#define SIGN_OF_X_BLS24479 POSITIVEX

#define ATE_BITS_BLS24479 49
#define G2_TABLE_BLS24479 52

#endif


#if CURVE_SECURITY_BLS24479 == 128
#define AESKEY_BLS24479 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_BLS24479 SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_BLS24479 == 192
#define AESKEY_BLS24479 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_BLS24479 SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_BLS24479 == 256
#define AESKEY_BLS24479 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_BLS24479 SHA512  /**< Hash type */
#endif


namespace BLS24479_BIG = B480_56;
namespace BLS24479_FP = BLS24479;

#endif
