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

#include "st7735s.h"
#include <stdio.h>

/* ============================================================
 * GPIO 操作宏
 * ============================================================ */
#define LCD_SCL_LOW()   HAL_GPIO_WritePin(LCD_SCL_GPIO_Port, LCD_SCL_Pin, GPIO_PIN_RESET)
#define LCD_SCL_HIGH()  HAL_GPIO_WritePin(LCD_SCL_GPIO_Port, LCD_SCL_Pin, GPIO_PIN_SET)
#define LCD_SDA_LOW()   HAL_GPIO_WritePin(LCD_SDA_GPIO_Port, LCD_SDA_Pin, GPIO_PIN_RESET)
#define LCD_SDA_HIGH()  HAL_GPIO_WritePin(LCD_SDA_GPIO_Port, LCD_SDA_Pin, GPIO_PIN_SET)
#define LCD_CS_LOW()    HAL_GPIO_WritePin(LCD_CS_GPIO_Port,  LCD_CS_Pin,  GPIO_PIN_RESET)
#define LCD_CS_HIGH()   HAL_GPIO_WritePin(LCD_CS_GPIO_Port,  LCD_CS_Pin,  GPIO_PIN_SET)
#define LCD_DC_CMD()    HAL_GPIO_WritePin(LCD_DC_GPIO_Port,  LCD_DC_Pin,  GPIO_PIN_RESET)
#define LCD_DC_DATA()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port,  LCD_DC_Pin,  GPIO_PIN_SET)
#define LCD_RES_LOW()   HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin, GPIO_PIN_RESET)
#define LCD_RES_HIGH()  HAL_GPIO_WritePin(LCD_RES_GPIO_Port, LCD_RES_Pin, GPIO_PIN_SET)
#define LCD_BL_ON()     HAL_GPIO_WritePin(LCD_BL_GPIO_Port,  LCD_BL_Pin,  GPIO_PIN_SET)
#define LCD_BL_OFF()    HAL_GPIO_WritePin(LCD_BL_GPIO_Port,  LCD_BL_Pin,  GPIO_PIN_RESET)

/* ============================================================
 * 软件 SPI 底层（Mode 0，SCL 上升沿采样）
 * ============================================================ */
static void SPI_WriteByte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++) {
        LCD_SCL_LOW();
        if (byte & 0x80) LCD_SDA_HIGH();
        else             LCD_SDA_LOW();
        byte <<= 1;
        LCD_SCL_HIGH();
    }
}

static void LCD_WriteCmd(uint8_t cmd)
{
    LCD_DC_CMD();
    LCD_CS_LOW();
    SPI_WriteByte(cmd);
    LCD_CS_HIGH();
}

static void LCD_WriteData8(uint8_t data)
{
    LCD_DC_DATA();
    LCD_CS_LOW();
    SPI_WriteByte(data);
    LCD_CS_HIGH();
}

/* ============================================================
 * 设置绘图窗口
 * ============================================================ */
void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    LCD_WriteCmd(0x2A);                               // CASET
    LCD_WriteData8(x0 >> 8); LCD_WriteData8(x0 & 0xFF);
    LCD_WriteData8(x1 >> 8); LCD_WriteData8(x1 & 0xFF);

    LCD_WriteCmd(0x2B);                               // RASET
    LCD_WriteData8(y0 >> 8); LCD_WriteData8(y0 & 0xFF);
    LCD_WriteData8(y1 >> 8); LCD_WriteData8(y1 & 0xFF);

    LCD_WriteCmd(0x2C);                               // RAMWR
}

/* ============================================================
 * 初始化序列
 * ============================================================ */
void ST7735S_Init(void)
{
    LCD_BL_OFF();
    LCD_RES_LOW();
    HAL_Delay(20);
    LCD_RES_HIGH();
    HAL_Delay(120);

    LCD_WriteCmd(0x01);  HAL_Delay(120);              // SWRESET
    LCD_WriteCmd(0x11);  HAL_Delay(120);              // SLPOUT

    LCD_WriteCmd(0xB1);                               // FRMCTR1
    LCD_WriteData8(0x01); LCD_WriteData8(0x2C); LCD_WriteData8(0x2D);
    LCD_WriteCmd(0xB2);
    LCD_WriteData8(0x01); LCD_WriteData8(0x2C); LCD_WriteData8(0x2D);
    LCD_WriteCmd(0xB3);
    LCD_WriteData8(0x01); LCD_WriteData8(0x2C); LCD_WriteData8(0x2D);
    LCD_WriteData8(0x01); LCD_WriteData8(0x2C); LCD_WriteData8(0x2D);
    LCD_WriteCmd(0xB4); LCD_WriteData8(0x07);         // INVCTR

    LCD_WriteCmd(0xC0);                               // PWCTR1
    LCD_WriteData8(0xA2); LCD_WriteData8(0x02); LCD_WriteData8(0x84);
    LCD_WriteCmd(0xC1); LCD_WriteData8(0xC5);         // PWCTR2
    LCD_WriteCmd(0xC2); LCD_WriteData8(0x0A); LCD_WriteData8(0x00);
    LCD_WriteCmd(0xC3); LCD_WriteData8(0x8A); LCD_WriteData8(0x2A);
    LCD_WriteCmd(0xC4); LCD_WriteData8(0x8A); LCD_WriteData8(0xEE);
    LCD_WriteCmd(0xC5); LCD_WriteData8(0x0E);         // VMCTR1

    LCD_WriteCmd(0x20);                               // INVOFF
    LCD_WriteCmd(0x36); LCD_WriteData8(0xC8);         // MADCTL 横屏+BGR
    LCD_WriteCmd(0x3A); LCD_WriteData8(0x05);         // COLMOD 16bit

    LCD_WriteCmd(0x2A);
    LCD_WriteData8(0x00); LCD_WriteData8(0x00);
    LCD_WriteData8(0x00); LCD_WriteData8(0x7F);
    LCD_WriteCmd(0x2B);
    LCD_WriteData8(0x00); LCD_WriteData8(0x00);
    LCD_WriteData8(0x00); LCD_WriteData8(0x9F);

    LCD_WriteCmd(0xE0);                               // GMCTRP1
    LCD_WriteData8(0x02); LCD_WriteData8(0x1C); LCD_WriteData8(0x07);
    LCD_WriteData8(0x12); LCD_WriteData8(0x37); LCD_WriteData8(0x32);
    LCD_WriteData8(0x29); LCD_WriteData8(0x2D); LCD_WriteData8(0x29);
    LCD_WriteData8(0x25); LCD_WriteData8(0x2B); LCD_WriteData8(0x39);
    LCD_WriteData8(0x00); LCD_WriteData8(0x01); LCD_WriteData8(0x03);
    LCD_WriteData8(0x10);

    LCD_WriteCmd(0xE1);                               // GMCTRN1
    LCD_WriteData8(0x03); LCD_WriteData8(0x1D); LCD_WriteData8(0x07);
    LCD_WriteData8(0x06); LCD_WriteData8(0x2E); LCD_WriteData8(0x2C);
    LCD_WriteData8(0x29); LCD_WriteData8(0x2D); LCD_WriteData8(0x2E);
    LCD_WriteData8(0x2E); LCD_WriteData8(0x37); LCD_WriteData8(0x3F);
    LCD_WriteData8(0x00); LCD_WriteData8(0x00); LCD_WriteData8(0x02);
    LCD_WriteData8(0x10);

    LCD_WriteCmd(0x13);  HAL_Delay(10);               // NORON
    LCD_WriteCmd(0x29);  HAL_Delay(100);              // DISPON

    LCD_BL_ON();
}

/* ============================================================
 * 批量填充矩形
 * ============================================================ */
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    LCD_SetWindow(x, y, x + w - 1, y + h - 1);
    LCD_DC_DATA();
    LCD_CS_LOW();
    uint32_t total = (uint32_t)w * h;
    for (uint32_t i = 0; i < total; i++) {
        SPI_WriteByte(color >> 8);
        SPI_WriteByte(color & 0xFF);
    }
    LCD_CS_HIGH();
}

void LCD_Clear(uint16_t color)
{
    LCD_FillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    LCD_SetWindow(x, y, x, y);
    LCD_DC_DATA();
    LCD_CS_LOW();
    SPI_WriteByte(color >> 8);
    SPI_WriteByte(color & 0xFF);
    LCD_CS_HIGH();
}

void LCD_WritePixels(const uint16_t *data, uint32_t len)
{
    LCD_DC_DATA();
    LCD_CS_LOW();
    for (uint32_t i = 0; i < len; i++) {
        SPI_WriteByte((data[i] >> 8) & 0xFF);
        SPI_WriteByte(data[i] & 0xFF);
    }
    LCD_CS_HIGH();
}

/* ============================================================
 * 5x7 ASCII 字体（列优先，bit0 在顶部）
 * 索引 = ASCII - 32
 * ============================================================ */
static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x00,0x00,0x5F,0x00,0x00}, // '!'
    {0x00,0x07,0x00,0x07,0x00}, // '"'
    {0x14,0x7F,0x14,0x7F,0x14}, // '#'
    {0x24,0x2A,0x7F,0x2A,0x12}, // '$'
    {0x23,0x13,0x08,0x64,0x62}, // '%'
    {0x36,0x49,0x55,0x22,0x50}, // '&'
    {0x00,0x05,0x03,0x00,0x00}, // '\''
    {0x00,0x1C,0x22,0x41,0x00}, // '('
    {0x00,0x41,0x22,0x1C,0x00}, // ')'
    {0x14,0x08,0x3E,0x08,0x14}, // '*'
    {0x08,0x08,0x3E,0x08,0x08}, // '+'
    {0x00,0x50,0x30,0x00,0x00}, // ','
    {0x08,0x08,0x08,0x08,0x08}, // '-'
    {0x00,0x60,0x60,0x00,0x00}, // '.'
    {0x20,0x10,0x08,0x04,0x02}, // '/'
    {0x3E,0x51,0x49,0x45,0x3E}, // '0'
    {0x00,0x42,0x7F,0x40,0x00}, // '1'
    {0x42,0x61,0x51,0x49,0x46}, // '2'
    {0x21,0x41,0x45,0x4B,0x31}, // '3'
    {0x18,0x14,0x12,0x7F,0x10}, // '4'
    {0x27,0x45,0x45,0x45,0x39}, // '5'
    {0x3C,0x4A,0x49,0x49,0x30}, // '6'
    {0x01,0x71,0x09,0x05,0x03}, // '7'
    {0x36,0x49,0x49,0x49,0x36}, // '8'
    {0x06,0x49,0x49,0x29,0x1E}, // '9'
    {0x00,0x36,0x36,0x00,0x00}, // ':'
    {0x00,0x56,0x36,0x00,0x00}, // ';'
    {0x08,0x14,0x22,0x41,0x00}, // '<'
    {0x14,0x14,0x14,0x14,0x14}, // '='
    {0x00,0x41,0x22,0x14,0x08}, // '>'
    {0x02,0x01,0x51,0x09,0x06}, // '?'
    {0x32,0x49,0x79,0x41,0x3E}, // '@'
    {0x7E,0x11,0x11,0x11,0x7E}, // 'A'
    {0x7F,0x49,0x49,0x49,0x36}, // 'B'
    {0x3E,0x41,0x41,0x41,0x22}, // 'C'
    {0x7F,0x41,0x41,0x22,0x1C}, // 'D'
    {0x7F,0x49,0x49,0x49,0x41}, // 'E'
    {0x7F,0x09,0x09,0x01,0x01}, // 'F'
    {0x3E,0x41,0x41,0x51,0x32}, // 'G'
    {0x7F,0x08,0x08,0x08,0x7F}, // 'H'
    {0x00,0x41,0x7F,0x41,0x00}, // 'I'
    {0x20,0x40,0x41,0x3F,0x01}, // 'J'
    {0x7F,0x08,0x14,0x22,0x41}, // 'K'
    {0x7F,0x40,0x40,0x40,0x40}, // 'L'
    {0x7F,0x02,0x04,0x02,0x7F}, // 'M'
    {0x7F,0x04,0x08,0x10,0x7F}, // 'N'
    {0x3E,0x41,0x41,0x41,0x3E}, // 'O'
    {0x7F,0x09,0x09,0x09,0x06}, // 'P'
    {0x3E,0x41,0x51,0x21,0x5E}, // 'Q'
    {0x7F,0x09,0x19,0x29,0x46}, // 'R'
    {0x46,0x49,0x49,0x49,0x31}, // 'S'
    {0x01,0x01,0x7F,0x01,0x01}, // 'T'
    {0x3F,0x40,0x40,0x40,0x3F}, // 'U'
    {0x1F,0x20,0x40,0x20,0x1F}, // 'V'
    {0x7F,0x20,0x18,0x20,0x7F}, // 'W'
    {0x63,0x14,0x08,0x14,0x63}, // 'X'
    {0x03,0x04,0x78,0x04,0x03}, // 'Y'
    {0x61,0x51,0x49,0x45,0x43}, // 'Z'
    {0x00,0x7F,0x41,0x41,0x00}, // '['
    {0x02,0x04,0x08,0x10,0x20}, // '\\'
    {0x00,0x41,0x41,0x7F,0x00}, // ']'
    {0x04,0x02,0x01,0x02,0x04}, // '^'
    {0x40,0x40,0x40,0x40,0x40}, // '_'
    {0x00,0x01,0x02,0x04,0x00}, // '`'
    {0x20,0x54,0x54,0x54,0x78}, // 'a'
    {0x7F,0x48,0x44,0x44,0x38}, // 'b'
    {0x38,0x44,0x44,0x44,0x20}, // 'c'
    {0x38,0x44,0x44,0x48,0x7F}, // 'd'
    {0x38,0x54,0x54,0x54,0x18}, // 'e'
    {0x08,0x7E,0x09,0x01,0x02}, // 'f'
    {0x08,0x14,0x54,0x54,0x3C}, // 'g'
    {0x7F,0x08,0x04,0x04,0x78}, // 'h'
    {0x00,0x44,0x7D,0x40,0x00}, // 'i'
    {0x20,0x40,0x44,0x3D,0x00}, // 'j'
    {0x00,0x7F,0x10,0x28,0x44}, // 'k'
    {0x00,0x41,0x7F,0x40,0x00}, // 'l'
    {0x7C,0x04,0x18,0x04,0x78}, // 'm'
    {0x7C,0x08,0x04,0x04,0x78}, // 'n'
    {0x38,0x44,0x44,0x44,0x38}, // 'o'
    {0x7C,0x14,0x14,0x14,0x08}, // 'p'
    {0x08,0x14,0x14,0x18,0x7C}, // 'q'
    {0x7C,0x08,0x04,0x04,0x08}, // 'r'
    {0x48,0x54,0x54,0x54,0x20}, // 's'
    {0x04,0x3F,0x44,0x40,0x20}, // 't'
    {0x3C,0x40,0x40,0x20,0x7C}, // 'u'
    {0x1C,0x20,0x40,0x20,0x1C}, // 'v'
    {0x3C,0x40,0x30,0x40,0x3C}, // 'w'
    {0x44,0x28,0x10,0x28,0x44}, // 'x'
    {0x0C,0x50,0x50,0x50,0x3C}, // 'y'
    {0x44,0x64,0x54,0x4C,0x44}, // 'z'
};

/* ============================================================
 * 绘制单个字符（6x8 单元，含 1px 间距）
 * ============================================================ */
void LCD_DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg)
{
    if (c < 32 || c > 122) c = ' ';
    uint8_t idx = c - 32;

    LCD_SetWindow(x, y, x + 5, y + 7);
    LCD_DC_DATA();
    LCD_CS_LOW();
    for (uint8_t row = 0; row < 8; row++) {
        for (uint8_t col = 0; col < 6; col++) {
            uint16_t color = bg;
            if (row < 7 && col < 5) {
                if (font5x7[idx][col] & (1 << row)) color = fg;
            }
            SPI_WriteByte(color >> 8);
            SPI_WriteByte(color & 0xFF);
        }
    }
    LCD_CS_HIGH();
}

void LCD_DrawString(uint16_t x, uint16_t y, const char *s, uint16_t fg, uint16_t bg)
{
    while (*s) {
        LCD_DrawChar(x, y, *s++, fg, bg);
        x += 6;
    }
}

void LCD_DrawNum(uint16_t x, uint16_t y, int num, uint16_t fg, uint16_t bg)
{
    char buf[12];
    snprintf(buf, sizeof(buf), "%d", num);
    LCD_DrawString(x, y, buf, fg, bg);
}

void LCD_DrawFloat(uint16_t x, uint16_t y, float num, uint8_t decimal, uint16_t fg, uint16_t bg)
{
    char buf[16];
    int neg = 0;
    if (num < 0) { neg = 1; num = -num; }
    int intpart = (int)num;
    int frac;
    if (decimal == 1)
        frac = (int)((num - intpart) * 10 + 0.5f);
    else
        frac = (int)((num - intpart) * 100 + 0.5f);

    if (decimal == 1)
        snprintf(buf, sizeof(buf), "%s%d.%d", neg ? "-" : "", intpart, frac);
    else
        snprintf(buf, sizeof(buf), "%s%d.%02d", neg ? "-" : "", intpart, frac);
    LCD_DrawString(x, y, buf, fg, bg);
}

/* ============================================================
 * 简单图形
 * ============================================================ */
void LCD_DrawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{
    LCD_FillRect(x, y, w, 1, color);
}

void LCD_DrawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color)
{
    LCD_FillRect(x, y, 1, h, color);
}

void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    LCD_DrawHLine(x,         y,         w, color);
    LCD_DrawHLine(x,         y + h - 1, w, color);
    LCD_DrawVLine(x,         y,         h, color);
    LCD_DrawVLine(x + w - 1, y,         h, color);
}

/* 画实心圆（Bresenham 扫描线算法） */
void LCD_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    int16_t x = 0;
    int16_t y = (int16_t)r;
    int16_t d = 3 - 2 * (int16_t)r;

    while (x <= y) {
        LCD_FillRect(x0 - x, y0 - y, 2 * x + 1, 1, color);
        LCD_FillRect(x0 - x, y0 + y, 2 * x + 1, 1, color);
        LCD_FillRect(x0 - y, y0 - x, 2 * y + 1, 1, color);
        LCD_FillRect(x0 - y, y0 + x, 2 * y + 1, 1, color);

        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

/* 画进度条：外框 + 百分比填充 */
void LCD_DrawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                         uint8_t percent, uint16_t fg, uint16_t bg)
{
    if (percent > 100) percent = 100;

    /* 外框 */
    LCD_DrawRect(x, y, w, h, WHITE);

    /* 内部区域 */
    uint16_t inner_w = w - 4;
    uint16_t inner_h = h - 4;

    /* 已填充部分 */
    uint16_t fill_w = (uint16_t)((uint32_t)inner_w * percent / 100);
    if (fill_w > 0) {
        LCD_FillRect(x + 2, y + 2, fill_w, inner_h, fg);
    }
    /* 未填充部分 */
    if (fill_w < inner_w) {
        LCD_FillRect(x + 2 + fill_w, y + 2, inner_w - fill_w, inner_h, bg);
    }
}