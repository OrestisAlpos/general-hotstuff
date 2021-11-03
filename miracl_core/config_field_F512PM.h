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

#ifndef CONFIG_FIELD_F512PM_H
#define CONFIG_FIELD_F512PM_H

#include"core.h"
#include "config_big_B512_60.h"

// FP stuff

#define MBITS_F512PM 512
#define PM1D2_F512PM 1
#define MODTYPE_F512PM PSEUDO_MERSENNE
#define MAXXES_F512PM 28
#define QNRI_F512PM 0
#define RIADZ_F512PM 0
#define RIADZG2A_F512PM 0
#define RIADZG2B_F512PM 0
#define TOWER_F512PM NEGATOWER

//#define BIG_ENDIAN_SIGN_F512PM 

#endif
