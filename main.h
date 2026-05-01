#ifndef _MAIN_H_
#define _MAIN_H_

/*================================== 头文件引用 =====================================*/
#include "ti_msp_dl_config.h"  /* TI MSPM0 芯片配置（系统时钟、外设配置等）*/
#include "stdio.h"              /* 标准输入输出（调试打印）*/
#include "clock.h"             /* 系统时钟和延时函数 */
#include "interrupt.h"          /* 中断处理函数 */

#include "oled_software_i2c.h" /* OLED显示屏驱动 */
#include "pwm.h"               /* PWM生成驱动 */
#include "motor.h"             /* 电机控制驱动 */
#include "Encoder.h"           /* 编码器读取驱动 */
#include "PID.h"               /* PID控制器驱动 */
#include "BlueSerial.h"        /* 蓝牙串口通信驱动 */
#include "wit.h"               /* 陀螺仪驱动 */
#include "grayscale_sensor.h"  /* 八路灰度传感器驱动 */
#include "Menu.h"              /* 菜单系统 */
#endif  /* #ifndef _MAIN_H_ */
