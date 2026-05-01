/**
 * @file pwm.h
 * @brief PWM输出驱动头文件
 *
 * 硬件说明：
 *   - PWM类型：定时器GPTimer产生PWM波
 *   - PWM通道：2个通道（通道0和通道1）
 *   - 通道0：左轮电机PWM
 *   - 通道1：右轮电机PWM
 *
 * PWM参数：
 *   - 频率：由 Set_Freq() 设置
 *   - 占空比：由 Set_Duty() 设置
 *   - 默认频率：PWM_MOTOR_INST_CLK_FREQ / 4000 ≈ 10kHz
 *
 * 占空比计算：
 *   - 占空比 = (1 - CCR/Period) × 100%
 *   - 当duty=1.0时，CCR=0，占空比100%
 *   - 当duty=0.0时，CCR=Period，占空比0%
 */

#ifndef PWM_H
#define PWM_H

/*================================== 头文件引用 =====================================*/
#include "ti_msp_dl_config.h"  /* TI MSPM0 芯片配置（定时器、PWM外设定义）*/

/*================================== 全局变量 =====================================*/
extern uint16_t Pwm_Count;

/*================================== 函数声明 =====================================*/

/**
 * @brief 设置PWM占空比
 *
 * @param n PWM通道编号：1=通道0（左轮），2=通道1（右轮）
 * @param duty 占空比，范围：0.0~1.0
 *           - duty=1.0 → 占空比100%（全速）
 *           - duty=0.5 → 占空比50%
 *           - duty=0.0 → 占空比0%（停止）
 *
 * @note 使用定时器的捕获比较寄存器实现PWM
 */
void Set_Duty(uint8_t n, float duty);

/**
 * @brief 设置PWM频率
 *
 * @param Freq 目标频率（Hz）
 *
 * @note 通过设置定时器周期值来改变PWM频率
 *       Period = 时钟频率 / 目标频率
 */
void Set_Freq(float Freq);

#endif /* PWM_H */
