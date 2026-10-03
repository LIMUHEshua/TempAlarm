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

#include "bh1750.h"
#include "dht11.h"   /* 用 delay_us */

#define SDA_HIGH()  HAL_GPIO_WritePin(BH1750_SDA_GPIO_Port, BH1750_SDA_Pin, GPIO_PIN_SET)
#define SDA_LOW()   HAL_GPIO_WritePin(BH1750_SDA_GPIO_Port, BH1750_SDA_Pin, GPIO_PIN_RESET)
#define SCL_HIGH()  HAL_GPIO_WritePin(BH1750_SCL_GPIO_Port, BH1750_SCL_Pin, GPIO_PIN_SET)
#define SCL_LOW()   HAL_GPIO_WritePin(BH1750_SCL_GPIO_Port, BH1750_SCL_Pin, GPIO_PIN_RESET)
#define SDA_READ()  HAL_GPIO_ReadPin (BH1750_SDA_GPIO_Port, BH1750_SDA_Pin)

static void I2C_Start(void)
{
    SDA_HIGH(); SCL_HIGH(); delay_us(5);
    SDA_LOW();  delay_us(5);
    SCL_LOW();  delay_us(5);
}

static void I2C_Stop(void)
{
    SDA_LOW();  SCL_HIGH(); delay_us(5);
    SDA_HIGH(); delay_us(5);
}

static uint8_t I2C_SendByte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++) {
        SCL_LOW(); delay_us(3);
        if (byte & 0x80) SDA_HIGH(); else SDA_LOW();
        byte <<= 1;
        delay_us(3);
        SCL_HIGH(); delay_us(5);
    }
    SCL_LOW(); delay_us(3);
    SDA_HIGH(); delay_us(3);
    SCL_HIGH(); delay_us(5);
    uint8_t ack = SDA_READ();
    SCL_LOW(); delay_us(3);
    return ack;
}

static uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t byte = 0;
    SDA_HIGH();
    for (uint8_t i = 0; i < 8; i++) {
        SCL_LOW(); delay_us(3);
        SCL_HIGH(); delay_us(5);
        byte <<= 1;
        if (SDA_READ()) byte |= 1;
    }
    SCL_LOW(); delay_us(3);
    if (ack) SDA_HIGH(); else SDA_LOW();
    delay_us(3);
    SCL_HIGH(); delay_us(5);
    SCL_LOW(); delay_us(3);
    return byte;
}

static uint8_t BH1750_WriteCmd(uint8_t cmd)
{
    I2C_Start();
    if (I2C_SendByte(BH1750_ADDR_W)) { I2C_Stop(); return 1; }
    if (I2C_SendByte(cmd))           { I2C_Stop(); return 2; }
    I2C_Stop();
    return 0;
}

static uint8_t BH1750_ReadRaw(uint16_t *raw)
{
    I2C_Start();
    if (I2C_SendByte(BH1750_ADDR_R)) { I2C_Stop(); return 1; }
    uint8_t high = I2C_ReadByte(0);
    uint8_t low  = I2C_ReadByte(1);
    I2C_Stop();
    *raw = ((uint16_t)high << 8) | low;
    return 0;
}

void BH1750_Init(void)
{
    SDA_HIGH();
    SCL_HIGH();
    HAL_Delay(10);
    BH1750_WriteCmd(BH1750_POWER_ON);
    HAL_Delay(10);
    BH1750_WriteCmd(BH1750_RESET);
    HAL_Delay(10);
    BH1750_WriteCmd(BH1750_CONT_H_RES);
    HAL_Delay(180);
}

uint8_t BH1750_ReadLight(float *lux)
{
    uint16_t raw;
    if (BH1750_ReadRaw(&raw) != 0) return 1;
    *lux = raw / 1.2f;
    return 0;
}