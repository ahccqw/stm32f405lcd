#ifndef _MAIN_H
#define _MAIN_H

/* ================= 核心库与标准库 ================= */
#include "stm32f4xx.h"    // STM32F4 系列标准外设库头文件
#include "stdio.h"        // 标准输入输出库 (用于 printf 等)
#include "string.h"       // 字符串处理函数库 (用于 memcpy, memset 等)

/* ================= 基础外设驱动 ================= */
#include "led.h"          // LED 驱动 (指示灯/照明)
#include "beep.h"         // 蜂鸣器驱动 (报警/提示音)
#include "key.h"          // 按键驱动 (用户输入)
#include "systick.h"      // 系统滴答定时器 (系统心跳/基础延时)
#include "delay.h"        // 软件延时函数

/* ================= 通信接口 (USART/CAN/SPI/IIC) ================= */
// --- 串口 ---
#include "usart1.h"       // 串口1 (通常用于调试打印或主通信)
#include "usart2wifi.h"   // 串口2 (连接 WIFI 模块)
#include "usart3spre.h"   // 串口3 (连接 传感器/从机设备)

// --- 总线 ---
#include "can.h"          // CAN 总线 (工业控制/汽车电子常用)
#include "iic.h"          // IIC 总线软件/硬件模拟 (连接传感器)
#include "spi1.h"         // SPI1 总线 (高速通信)

/* ================= 传感器与输入设备 ================= */
#include "adc1.h"         // ADC1 (模数转换，采集模拟电压)
#include "tphmsensor.h"   // 温湿度传感器 (可能是 SHT30/DHT11 等)
#include "infrared_tp.h"  // 红外触摸屏/红外传感器
#include "touch.h"        // 电容触摸屏驱动

/* ================= 电机与控制 ================= */
#include "motor.h"        // 直流电机/步进电机驱动
#include "servo_motor.h"  // 舵机驱动 (角度控制)
#include "time6.h"        // 定时器6 (可能用于电机 PWM 生成或输入捕获)

/* ================= 存储与文件系统 (Flash/SD) ================= */
#include "w25q64.h"       // 外部 Flash 芯片驱动 (W25Q64, 8MB)
#include "sd_driver.h"    // SD 卡底层驱动
#include "diskio.h"       // FatFs 磁盘 I/O 接口层
#include "ff.h"           // FatFs 文件系统核心头文件

/* ================= 显示与多媒体 (LCD/Audio) ================= */
#include "lcd.h"          // LCD 显示屏驱动 (TFT-LCD)
#include "ws2812s.h"      // WS2812 彩灯驱动 (RGB LED)
#include "wm8978.h"       // 音频编解码芯片驱动 (WM8978)
#include "music_ct.h"     // 音乐控制/播放逻辑
#include "audioplay.h"    // 音频播放管理

/* ================= 系统工具与算法 ================= */
#include "dma.h"          // DMA 直接存储器访问 (提高数据传输效率)
#include "rtc.h"          // 实时时钟 (日历功能)
#include "crc_table.h"    // CRC 校验算法表 (用于 Modbus/通信校验)
#include "mymalloc.h"     // 内存管理 (动态内存分配)
#include "exfuns.h"       // 扩展功能函数 (通常是文件系统或系统辅助函数)

/* ================= 项目特定业务逻辑 ================= */
#include "sterilize.h"    // 消毒功能逻辑 (项目特有业务)
#include "rs485_modbus_master.h" // Modbus RTU 协议栈 (主/从机逻辑)
#include "tim7_7ms.h"     // TIM7 定时器 (用于 Modbus 3.5ms 超时判断)

#endif
