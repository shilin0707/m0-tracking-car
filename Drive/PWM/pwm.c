/**
 * @file pwm.c
 * @brief PWM输出驱动源文件
 *
 * PWM工作原理：
 *   - 定时器产生周期信号，计数器从0递增到Period值
 *   - 捕获比较寄存器(CCR)决定输出高低电平的切换点
 *   - 当计数器<CCR时输出一种电平，>CCR时输出另一种电平
 *
 * 占空比计算：
 *   - PWM使用反向模式：CCR=Period×(1-duty)
 *   - duty=1.0 → CCR=0 → 始终高电平（占空比100%）
 *   - duty=0.5 → CCR=Period/2 → 高电平半个周期（占空比50%）
 *   - duty=0.0 → CCR=Period → 始终低电平（占空比0%）
 *
 * 通道分配：
 *   - 通道0(C0)：左轮电机
 *   - 通道1(C1)：右轮电机
 */

#include "pwm.h"

/*================================== 全局变量 =====================================*/
/**
 * @brief PWM周期计数器值
 * @note 决定PWM频率：Freq = CLK_FREQ / Pwm_Count
 *       默认4000对应约10kHz（假设CLK=40MHz）
 */
uint16_t Pwm_Count = 4000;

/*================================== PWM控制函数 =====================================*/

/**
 * @brief 设置PWM占空比
 *
 * @param n PWM通道编号
 *        - n=1: 通道0，左轮电机
 *        - n=2: 通道1，右轮电机
 * @param duty 占空比，范围：0.0~1.0
 *
 * 占空比设置原理：
 *   - 使用定时器的捕获比较寄存器(CCR)控制输出
 *   - CCR值 = Period × (1 - duty)
 *   - 由于使用反向PWM模式，CCR越小占空比越大
 *
 * 示例（Pwm_Count=4000）：
 *   - duty=1.0 → CCR=0 → 占空比100%（全速）
 *   - duty=0.5 → CCR=2000 → 占空比50%
 *   - duty=0.1 → CCR=3600 → 占空比10%（低速）
 */
void Set_Duty(uint8_t n, float duty)
{
    /* n=1 通道0 左轮 */
    if (n == 1)
    {
        /* 计算CCR值：CCR = Period × (1 - duty) */
        uint32_t CCR = (uint32_t)Pwm_Count * (1 - duty);
        /* 设置通道0的比较寄存器值 */
        DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST, CCR, GPIO_PWM_MOTOR_C0_IDX);
    }
    /* n=2 通道1 右轮 */
    else if (n == 2)
    {
        /* 计算CCR值 */
        uint32_t CCR = (uint32_t)Pwm_Count * (1 - duty);
        /* 设置通道1的比较寄存器值 */
        DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST, CCR, GPIO_PWM_MOTOR_C1_IDX);
    }
}

/**
 * @brief 设置PWM频率
 *
 * @param Freq 目标频率（Hz）
 *
 * 频率计算原理：
 *   - 周期值(Period) = 时钟频率 / 目标频率
 *   - Pwm_Count = PWM_MOTOR_INST_CLK_FREQ / Freq
 *
 * 示例（CLK=40MHz）：
 *   - Freq=10000 → Pwm_Count=4000 → 周期0.1ms，频率10kHz
 *   - Freq=1000 → Pwm_Count=40000 → 周期1ms，频率1kHz
 */
void Set_Freq(float Freq)
{
    /* 计算周期值 */
    Pwm_Count = PWM_MOTOR_INST_CLK_FREQ / Freq;
    /* 设置定时器周期值 */
    DL_Timer_setLoadValue(PWM_MOTOR_INST, Pwm_Count);
}
