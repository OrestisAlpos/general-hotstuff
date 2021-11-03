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

#ifndef CONFIG_FIELD_F256PME_H
#define CONFIG_FIELD_F256PME_H

#include"core.h"
#include "config_big_B256_56.h"

// FP stuff

#define MBITS_F256PME 256
#define PM1D2_F256PME 1
#define MODTYPE_F256PME PSEUDO_MERSENNE
#define MAXXES_F256PME 24
#define QNRI_F256PME 0
#define RIADZ_F256PME 0
#define RIADZG2A_F256PME 0
#define RIADZG2B_F256PME 0
#define TOWER_F256PME NEGATOWER

//#define BIG_ENDIAN_SIGN_F256PME 

#endif
