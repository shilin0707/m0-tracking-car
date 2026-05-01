/**
 * @file motor.h
 * @brief 直流电机驱动头文件
 *
 * 硬件配置：
 *   - 电机A（右侧）：PWM通道2，方向控制引脚A_IN1/A_IN2
 *   - 电机B（左侧）：PWM通道1，方向控制引脚B_IN1/B_IN2
 *
 * 使用说明：
 *   - Motor_SetPWM(n, Duty)：设置电机PWM，n=1左轮，n=2右轮，Duty=占空比(0~1)
 *   - Motor_Speed()：计算当前电机实际速度（mm/s），存入Speed_L/Speed_R
 *
 * 注意：
 *   - Speed_L/Speed_R 是全局变量，由 Motor_Speed() 更新
 *   - 编码器相关定义在 Encoder.h 中
 */

#ifndef MOTOR_H
#define MOTOR_H

/*================================== 头文件引用 =====================================*/
#include "ti_msp_dl_config.h"  /* TI MSPM0 芯片配置（GPIO引脚定义等）*/
#include "math.h"             /* 数学函数（PI常数等）*/
#include "stdlib.h"           /* 标准库（abs函数等）*/
#include "pwm.h"              /* PWM输出驱动 */
#include "Encoder.h"          /* 编码器读取驱动 */

/*================================== 宏定义 =====================================*/
#define PI 3.14159265f          /**< 圆周率，用于计算轮子周长 */
#define CIRCLE 728.0f           /**< 编码器每转脉冲数（AB相4倍频后） */
#define DIAMETER 65.0f           /**< 轮子直径，单位：mm */
#define TIM_FRE 100              /**< 速度采样频率，单位：Hz（每10ms采样一次） */
#define K 0.2f                   /**< 一阶滤波系数，范围[0,1]，越大越跟随瞬时值 */
#define ENCODER_OVERFLOW_MAX  1073741827.0f  /**< 编码器计数器溢出阈值（32位有符号） */
#define DIFF_THRESHOLD (ENCODER_OVERFLOW_MAX / 4)  /**< 差值阈值：防止异常采样跳变 */

/*================================== 全局变量 =====================================*/
/**
 * @brief 电机实际速度（单位：mm/s）
 * @note 由 Motor_Speed() 函数计算更新
 */
extern float Speed_L;   /**< 左轮实际速度 */
extern float Speed_R;   /**< 右轮实际速度 */

/*================================== 函数声明 =====================================*/

/**
 * @brief 直流电机设置PWM
 *
 * @param n 电机编号：1=左轮(B电机)，2=右轮(A电机)
 * @param Duty PWM占空比，范围：0.0~1.0（正数=正转，负数=反转）
 *
 * @note 调用此函数后电机不会立即转动，需等待下一周期电机定时器中断更新PWM输出
 */
void Motor_SetPWM(uint8_t n, float Duty);

/**
 * @brief 电机速度计算
 *
 * 功能说明：
 *   - 读取编码器当前值
 *   - 计算与上次的差值
 *   - 进行跨零修正
 *   - 转换为实际速度（mm/s）
 *   - 一阶滤波平滑
 *
 * @note 此函数由电机定时器中断调用，更新 Speed_L 和 Speed_R 全局变量
 */
void Motor_Speed(void);

#endif /* MOTOR_H */
