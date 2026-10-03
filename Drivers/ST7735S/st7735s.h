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

#ifndef ST7735S_H
#define ST7735S_H

#include "main.h"

#define LCD_WIDTH   128
#define LCD_HEIGHT  160

#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define ORANGE  0xFD20
#define GRAY    0x8410

void ST7735S_Init(void);
void LCD_Clear(uint16_t color);
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_WritePixels(const uint16_t *data, uint32_t len);

void LCD_DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg);
void LCD_DrawString(uint16_t x, uint16_t y, const char *s, uint16_t fg, uint16_t bg);
void LCD_DrawNum(uint16_t x, uint16_t y, int num, uint16_t fg, uint16_t bg);
void LCD_DrawFloat(uint16_t x, uint16_t y, float num, uint8_t decimal, uint16_t fg, uint16_t bg);

void LCD_DrawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
void LCD_DrawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void LCD_DrawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                         uint8_t percent, uint16_t fg, uint16_t bg);

#endif /* ST7735S_H */