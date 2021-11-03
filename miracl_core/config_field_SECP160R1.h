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

#ifndef CONFIG_FIELD_SECP160R1_H
#define CONFIG_FIELD_SECP160R1_H

#include"core.h"
#include "config_big_B160_56.h"

// FP stuff

#define MBITS_SECP160R1 160
#define PM1D2_SECP160R1 1
#define MODTYPE_SECP160R1 NOT_SPECIAL
#define MAXXES_SECP160R1 8
#define QNRI_SECP160R1 0
#define RIADZ_SECP160R1 3
#define RIADZG2A_SECP160R1 0
#define RIADZG2B_SECP160R1 0
#define TOWER_SECP160R1 NEGATOWER

//#define BIG_ENDIAN_SIGN_SECP160R1 

#endif
