/*
 * Copyright (c) 2026 何少华 (LIMUHEshua). All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * 说明：本文件骨架由 STM32CubeMX 生成，业务逻辑与修改部分
 *       版权归何少华所有，采用 Apache License 2.0 授权。
 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.c
  * @brief   主程序 - STM32F103C8T6 + DHT11 + BH1750 + ST7735S + FreeRTOS
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "dht11.h"
#include "st7735s.h"
#include "bh1750.h"

#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define TEMP_THRESHOLD      35.0f
#define TEMP_HYSTERESIS     2.0f
#define DHT_ERR_MAX         3

#define EVT_ALARM_ACTIVE    (1 << 0)

#define TASK_SENSOR_STACK   256
#define TASK_ALARM_STACK    192
#define TASK_DISPLAY_STACK  256
#define TASK_LED_STACK      96

#define TASK_SENSOR_PRIO    2
#define TASK_ALARM_PRIO     3
#define TASK_DISPLAY_PRIO   1
#define TASK_LED_PRIO       4

/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

typedef struct {
    float   temp;
    float   humi;
    float   lux;
    uint8_t alarm;
    uint8_t fault;
} SensorData_t;

static QueueHandle_t      xQueue_Sensor;
static EventGroupHandle_t xEvent_Alarm;
static TaskHandle_t       xTask_Alarm   = NULL;
static TaskHandle_t       xTask_Display = NULL;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
static void Task_Sensor(void *arg);
static void Task_Alarm(void *arg);
static void Task_Display(void *arg);
static void Task_LED(void *arg);
static void UI_Init(void);
static void UI_ShowData(SensorData_t *d);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void UI_Init(void)
{
    LCD_Clear(BLACK);

    LCD_DrawRect(0, 2, 128, 14, CYAN);
    LCD_DrawString(8, 6, "TEMP HUMI LIGHT", CYAN, BLACK);

    LCD_DrawString(4, 22, "TEMP:", WHITE, BLACK);
    LCD_DrawString(100, 22, "C", YELLOW, BLACK);
    LCD_DrawString(4, 50, "HUMI:", WHITE, BLACK);
    LCD_DrawString(100, 50, "%", GREEN, BLACK);
    LCD_DrawString(4, 78, "LUX: ", WHITE, BLACK);
    LCD_DrawString(100, 78, "lx", CYAN, BLACK);

    LCD_DrawRect(0, 100, 128, 16, GRAY);
}

static void UI_ShowData(SensorData_t *d)
{
    /* 温度 */
    LCD_FillRect(46, 22, 50, 8, BLACK);
    LCD_DrawFloat(46, 22, d->temp, 1, YELLOW, BLACK);
    uint8_t p = (uint8_t)(d->temp * 2);
    if (p > 100) p = 100;
    uint16_t bar_color = (d->temp >= TEMP_THRESHOLD) ? RED : YELLOW;
    LCD_DrawProgressBar(4, 34, 120, 10, p, bar_color, BLACK);

    /* 湿度 */
    LCD_FillRect(46, 50, 50, 8, BLACK);
    LCD_DrawFloat(46, 50, d->humi, 1, GREEN, BLACK);
    p = (uint8_t)d->humi;
    if (p > 100) p = 100;
    LCD_DrawProgressBar(4, 62, 120, 10, p, GREEN, BLACK);

    /* 光照 */
    LCD_FillRect(46, 78, 82, 8, BLACK);
    LCD_DrawNum(46, 78, (int)d->lux, CYAN, BLACK);

    /* 状态行 */
    LCD_FillRect(3, 103, 122, 10, BLACK);
    if (d->fault) {
        LCD_FillCircle(12, 108, 4, RED);
        LCD_DrawString(24, 104, "SENSOR ERR", RED, BLACK);
    } else if (d->alarm) {
        LCD_FillCircle(12, 108, 4, RED);
        LCD_DrawString(24, 104, "ALARM!    ", RED, BLACK);
    } else {
        LCD_FillCircle(12, 108, 4, GREEN);
        LCD_DrawString(24, 104, "NORMAL    ", GREEN, BLACK);
    }
}

/* ============================================================
 *  Task_Sensor
 * ============================================================ */
static void Task_Sensor(void *arg)
{
    (void)arg;
    SensorData_t data = {0};
    uint8_t  dht_err_cnt = 0;
    TickType_t dht_tick = xTaskGetTickCount();
    TickType_t lux_tick = dht_tick;

    for (;;) {
        TickType_t now = xTaskGetTickCount();

        if ((now - dht_tick) >= pdMS_TO_TICKS(1000)) {
            dht_tick = now;

            float t, h;
            if (DHT11_ReadData(&t, &h) == 0) {
                dht_err_cnt = 0;
                data.temp = t;
                data.humi = h;
                data.fault = 0;
            } else {
                if (dht_err_cnt < 255) dht_err_cnt++;
                if (dht_err_cnt >= DHT_ERR_MAX) data.fault = 1;
            }

            if (!data.alarm) {
                if (data.temp >= TEMP_THRESHOLD) data.alarm = 1;
            } else {
                if (data.temp < (TEMP_THRESHOLD - TEMP_HYSTERESIS)) data.alarm = 0;
            }
            if (data.fault) data.alarm = 0;

            xQueueOverwrite(xQueue_Sensor, &data);
            xTaskNotifyGive(xTask_Alarm);
            xTaskNotifyGive(xTask_Display);
        }

        if ((now - lux_tick) >= pdMS_TO_TICKS(2000)) {
            lux_tick = now;
            float l;
            if (BH1750_ReadLight(&l) == 0) {
                data.lux = l;
                xQueueOverwrite(xQueue_Sensor, &data);
                xTaskNotifyGive(xTask_Display);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* ============================================================
 *  Task_Alarm
 * ============================================================ */
static void Task_Alarm(void *arg)
{
    (void)arg;
    SensorData_t data;

    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (xQueuePeek(xQueue_Sensor, &data, 0) == pdTRUE) {
            if (data.alarm) {
                HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
                xEventGroupSetBits(xEvent_Alarm, EVT_ALARM_ACTIVE);
            } else {
                HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
                xEventGroupClearBits(xEvent_Alarm, EVT_ALARM_ACTIVE);
                HAL_GPIO_WritePin(LED_HW269_GPIO_Port, LED_HW269_Pin, GPIO_PIN_RESET);
            }
        }
    }
}

/* ============================================================
 *  Task_Display
 * ============================================================ */
static void Task_Display(void *arg)
{
    (void)arg;
    SensorData_t data;

    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (xQueuePeek(xQueue_Sensor, &data, 0) == pdTRUE) {
            UI_ShowData(&data);
        }
    }
}

/* ============================================================
 *  Task_LED
 * ============================================================ */
static void Task_LED(void *arg)
{
    (void)arg;
    for (;;) {
        EventBits_t bits = xEventGroupWaitBits(xEvent_Alarm,
                                               EVT_ALARM_ACTIVE,
                                               pdFALSE, pdFALSE,
                                               pdMS_TO_TICKS(250));
        if (bits & EVT_ALARM_ACTIVE) {
            HAL_GPIO_TogglePin(LED_HW269_GPIO_Port, LED_HW269_Pin);
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask; (void)pcTaskName;
    __disable_irq();
    while (1);
}

void vApplicationMallocFailedHook(void)
{
    __disable_irq();
    while (1);
}

/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    ST7735S_Init();
    UI_Init();

    DHT11_Init();
    BH1750_Init();

    xQueue_Sensor = xQueueCreate(1, sizeof(SensorData_t));
    xEvent_Alarm  = xEventGroupCreate();

    if (xQueue_Sensor == NULL || xEvent_Alarm == NULL) {
        Error_Handler();
    }

    xTaskCreate(Task_Sensor,  "Sensor",  TASK_SENSOR_STACK,  NULL,
                TASK_SENSOR_PRIO,  NULL);
    xTaskCreate(Task_Alarm,   "Alarm",   TASK_ALARM_STACK,   NULL,
                TASK_ALARM_PRIO,   &xTask_Alarm);
    xTaskCreate(Task_Display, "Display", TASK_DISPLAY_STACK, NULL,
                TASK_DISPLAY_PRIO, &xTask_Display);
    xTaskCreate(Task_LED,     "LED",     TASK_LED_STACK,     NULL,
                TASK_LED_PRIO,     NULL);

    vTaskStartScheduler();

    while (1) {}
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
      Error_Handler();
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif