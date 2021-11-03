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

#ifndef CONFIG_FIELD_NIST256_H
#define CONFIG_FIELD_NIST256_H

#include"core.h"
#include "config_big_B256_56.h"

// FP stuff

#define MBITS_NIST256 256
#define PM1D2_NIST256 1
#define MODTYPE_NIST256 NOT_SPECIAL
#define MAXXES_NIST256 24
#define QNRI_NIST256 0
#define RIADZ_NIST256 -10
#define RIADZG2A_NIST256 0
#define RIADZG2B_NIST256 0
#define TOWER_NIST256 NEGATOWER

//#define BIG_ENDIAN_SIGN_NIST256 

#endif
