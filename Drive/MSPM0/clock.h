#ifndef _CLOCK_H_
#define _CLOCK_H_

/*================================== 头文件引用 =====================================*/
#include "ti_msp_dl_config.h"  /* TI MSPM0 芯片配置（GPIO、UART等外设定义）*/

extern volatile unsigned long tick_ms;

int mspm0_delay_ms(unsigned long num_ms);
int mspm0_get_clock_ms(unsigned long *count);
void SysTick_Init(void);

#endif  /* #ifndef _CLOCK_H_ */