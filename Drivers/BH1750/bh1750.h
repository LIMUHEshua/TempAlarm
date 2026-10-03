/*
 * Copyright (c) 2026 何少华 (LIMUHEshua). All rights reserved.
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
 *
 * 项目：资源受限平台下的多传感器监测终端
 * 作者：何少华
 * 仓库：https://github.com/LIMUHEshua/TempAlarm
 */

#ifndef BH1750_H
#define BH1750_H

#include "main.h"

#define BH1750_ADDR_W   0x46
#define BH1750_ADDR_R   0x47

#define BH1750_POWER_ON     0x01
#define BH1750_RESET        0x07
#define BH1750_CONT_H_RES   0x10

void    BH1750_Init(void);
uint8_t BH1750_ReadLight(float *lux);

#endif /* BH1750_H */