# TempAlarm

STM32F103C8T6 + FreeRTOS 温湿度/光照监测终端项目，基于 STM32CubeMX 生成基础工程，并使用 CMake + VSCode 进行构建和开发。

## 项目简介

TempAlarm 是一个面向嵌入式环境监测场景的项目，适用于温湿度和光照监测。该项目使用 STM32F103C8T6 作为主控芯片，结合 FreeRTOS 实现多任务调度，使用 DHT11 温湿度传感器、BH1750 光照传感器和 ST7735S TFT 液晶屏进行数据采集与显示。

## 功能说明

- DHT11 温湿度采集
- BH1750 光照强度采集
- ST7735S 屏幕显示
- FreeRTOS 多任务调度
- 报警状态管理
- CMake 构建环境
- VSCode 开发调试支持

## 硬件平台

- MCU: STM32F103C8T6
- RTOS: FreeRTOS
- 温湿度传感器: DHT11
- 光照传感器: BH1750
- 显示屏: ST7735S 1.8" TFT
- 其它: LED、蜂鸣器等状态指示与报警模块

## 硬件接线说明

以下为参考接线，实际接线请以原理图和板卡设计为准。

- DHT11 DATA -> PAx
- BH1750 SDA -> PB7 / I2C1_SDA
- BH1750 SCL -> PB6 / I2C1_SCL
- ST7735S CS -> PA4
- ST7735S DC -> PA3
- ST7735S RST -> PA2
- ST7735S BL -> PA1
- LED -> PC13
- BUZZER -> PB8

> 硬件设计、原理图、PCB 及接线方案仅供参考，未经授权不得直接用于商业生产。

## 传感器说明

### DHT11

DHT11 用于测量环境温度和湿度，适合室内温湿度监测。数据采集周期一般由任务调度控制，读取结果用于显示和报警判断。

### BH1750

BH1750 是一款数字光照传感器，支持 I2C 接口，适用于环境亮度统计和光照阈值判定。

## FreeRTOS 任务说明

项目采用 FreeRTOS 运行多任务，常见任务包括：

- 传感器采集任务
- 数据处理任务
- 屏幕刷新任务
- 报警判断任务
- 状态指示任务

任务间可以通过队列、事件组或任务通知实现通信和同步。

## 构建步骤

1. 安装 ARM GCC 工具链
2. 安装 CMake 与 Ninja
3. 打开项目根目录
4. 配置构建：

```bash
cmake -S . -B build -G Ninja
```

5. 编译：

```bash
cmake --build build
```

## 烧录步骤

1. 使用 ST-Link / J-Link 等调试器连接 MCU
2. 生成固件文件（例如 .elf 或 .bin）
3. 使用 STM32CubeProgrammer 或其他烧录工具写入程序
4. 断开调试器后重新上电运行

## 目录结构

```text
TempAlarm/
├── CMakeLists.txt
├── CMakePresets.json
├── LICENSE
├── README.md
├── CHANGELOG.md
├── CONTRIBUTING.md
├── startup_stm32f103xb.s
├── STM32F103XX_FLASH.ld
├── TempAlarm.ioc
├── cmake/
├── Core/
├── Drivers/
├── build/
├── .vscode/
├── hardware/
└── .github/
```

## 开源协议说明

本项目采用 Apache License 2.0。详见 [LICENSE](LICENSE)。

## 硬件设计免责声明

本项目中所涉及的硬件设计、原理图、PCB 和接线方案仅供学习、参考和实验用途，不构成任何商业许可或生产授权。任何未经授权的直接用于商业生产、量产或工程落地，均由使用者自行承担责任。

## 贡献说明

欢迎提交 Issue、修复缺陷、改进文档或加入功能开发。请先阅读 [CONTRIBUTING.md](CONTRIBUTING.md) 以了解贡献流程。

## 维护说明

此仓库适合用于学习 STM32 + FreeRTOS + CMake 的嵌入式开发工作流。请在使用前确认适配的开发环境和目标硬件版本。
