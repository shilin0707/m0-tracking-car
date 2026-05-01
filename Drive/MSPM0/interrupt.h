#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

/*================================== 头文件引用 =====================================*/
#include "ti_msp_dl_config.h"  /* TI MSPM0 芯片配置（中断控制器等）*/
#include "clock.h"             /* 系统时钟和延时函数 */
#include "motor.h"            /* 电机驱动（中断处理函数中使用）*/
#include "oled_software_i2c.h" /* OLED显示（中断处理函数中使用）*/
#include "wit.h"              /* 陀螺仪数据（中断处理函数中使用）*/

extern uint8_t enable_group1_irq;

void Interrupt_Init(void);

#endif  /* #ifndef _INTERRUPT_H_ */