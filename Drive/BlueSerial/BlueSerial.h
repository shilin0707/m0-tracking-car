#ifndef __BLUESERIAL_H__
#define __BLUESERIAL_H__

/*================================== 头文件引用 =====================================*/
#include "ti_msp_dl_config.h"  /* TI MSPM0 芯片配置（UART外设定义等）*/
#include "motor.h"            /* 电机驱动（目标速度变量等）*/
#include <stdio.h>           /* 标准输入输出（printf等）*/
#include <stdarg.h>          /* 可变参数处理（va_list等）*/
#include <string.h>          /* 字符串处理（strtok等）*/
#include <stdlib.h>          /* 标准库（atoi、atof等）*/
#include "math.h"            /* 数学函数 */

extern char BlueSerial_RxPacket[];
extern uint8_t BlueSerial_RxFlag;

extern char BlueSerial_RxPacket[100];
extern uint8_t BlueSerial_RxFlag;

/* BlueSerial控制模式相关变量声明 */
extern uint8_t g_blueserial_control_enable;  /* BlueSerial控制模式标志 */
extern float g_target_speed_L;              /* 左轮目标速度 */
extern float g_target_speed_R;              /* 右轮目标速度 */

void BlueSerial_SendByte(uint8_t Byte);
void BlueSerial_SendArray(uint8_t *Array, uint16_t Length);
void BlueSerial_SendString(char *String);
uint32_t BlueSerial_Pow(uint32_t X, uint32_t Y);
void BlueSerial_SendNumber(uint32_t Number, uint8_t Length);
void BlueSerial_Tick(char *BlueSerial_RxPacket);
void BlueSerial_Printf(char *format, ...);
void BlueSerial_Control(int8_t LV, int8_t RV);
void BlueSerial_Control_Stop(void);

#endif
