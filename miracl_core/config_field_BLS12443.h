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

#ifndef CONFIG_FIELD_BLS12443_H
#define CONFIG_FIELD_BLS12443_H

#include"core.h"
#include "config_big_B448_60.h"

// FP stuff

#define MBITS_BLS12443 443
#define PM1D2_BLS12443 1
#define MODTYPE_BLS12443 NOT_SPECIAL
#define MAXXES_BLS12443 30
#define QNRI_BLS12443 0
#define RIADZ_BLS12443 -7
#define RIADZG2A_BLS12443 1
#define RIADZG2B_BLS12443 1
#define TOWER_BLS12443 NEGATOWER

//#define BIG_ENDIAN_SIGN_BLS12443 

#endif
