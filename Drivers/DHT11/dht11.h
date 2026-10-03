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

#ifndef DHT11_H
#define DHT11_H

#include "main.h"

void    DHT11_Init(void);
uint8_t DHT11_ReadData(float *temp, float *humi);

/* 微秒级延时，供 BH1750 软件 I2C 调用 */
void    delay_us(uint32_t us);

#endif /* DHT11_H */