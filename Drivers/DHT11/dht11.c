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

#include "dht11.h"
#include "FreeRTOS.h"
#include "task.h"

/* ---- GPIO 操作宏 ---- */
#define DHT11_LOW()   HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_RESET)
#define DHT11_HIGH()  HAL_GPIO_WritePin(DHT11_DATA_GPIO_Port, DHT11_DATA_Pin, GPIO_PIN_SET)
#define DHT11_READ()  HAL_GPIO_ReadPin (DHT11_DATA_GPIO_Port, DHT11_DATA_Pin)

/* ---- DWT 微秒延时 ---- */
static void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks);
}

/* ---- 等待引脚变电平，超时返回 1 ---- */
static uint8_t DHT11_WaitLevel(uint8_t level, uint32_t timeout_us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = timeout_us * (SystemCoreClock / 1000000U);
    while (DHT11_READ() != level) {
        if ((DWT->CYCCNT - start) > ticks) return 1;
    }
    return 0;
}

void DHT11_Init(void)
{
    DWT_Init();
    DHT11_HIGH();
    HAL_Delay(1000);
}

uint8_t DHT11_ReadData(float *temp, float *humi)
{
    uint8_t data[5] = {0};
    uint8_t i, j;

    /* 1. 主机起始信号 */
    DHT11_LOW();
    HAL_Delay(20);
    DHT11_HIGH();
    delay_us(30);

    /* 2. 关中断，防止 FreeRTOS 调度打断时序 */
    taskENTER_CRITICAL();

    if (DHT11_WaitLevel(0, 100)) { taskEXIT_CRITICAL(); return 1; }
    if (DHT11_WaitLevel(1, 100)) { taskEXIT_CRITICAL(); return 2; }
    if (DHT11_WaitLevel(0, 100)) { taskEXIT_CRITICAL(); return 3; }

    /* 3. 读 40 位 */
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
            if (DHT11_WaitLevel(1, 100)) { taskEXIT_CRITICAL(); return 4; }
            delay_us(40);
            data[i] <<= 1;
            if (DHT11_READ()) data[i] |= 1;
            if (DHT11_WaitLevel(0, 100)) { taskEXIT_CRITICAL(); return 5; }
        }
    }

    taskEXIT_CRITICAL();

    /* 4. 校验 */
    uint8_t sum = data[0] + data[1] + data[2] + data[3];
    if (sum != data[4]) return 6;

    *humi = data[0] + data[1] * 0.1f;
    *temp = data[2] + data[3] * 0.1f;
    return 0;
}