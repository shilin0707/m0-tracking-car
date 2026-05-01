/**
 * @file Buzzer.c
 * @brief 蜂鸣器驱动源文件
 *
 * 工作原理：
 *   - 无源蜂鸣器需要交流信号驱动
 *   - 此处使用GPIO模拟PWM：快速开关产生振动
 *   - 对于有源蜂鸣器，只需GPIO高低电平控制
 *
 * 提示音功能：
 *   - Buzzer_Delay_ON() 用于产生短暂提示音
 *   - 延时时间由 DELAY_TIME 定义
 */

#include "Buzzer.h"

/*================================== 蜂鸣器控制函数 =====================================*/

/**
 * @brief 打开蜂鸣器
 * @note 设置GPIO引脚为高电平，蜂鸣器持续响
 */
void Buzzer_ON(void)
{
    DL_GPIO_setPins(GPIO_Buzzer_PORT, GPIO_Buzzer_PIN_Buzzer_PIN);
}

/**
 * @brief 关闭蜂鸣器
 * @note 设置GPIO引脚为低电平，蜂鸣器停止
 */
void Buzzer_OFF(void)
{
    DL_GPIO_clearPins(GPIO_Buzzer_PORT, GPIO_Buzzer_PIN_Buzzer_PIN);
}

/**
 * @brief 蜂鸣器提示音
 * @note 蜂鸣器响一段时间后自动关闭，用于按键提示
 *
 * 执行流程：
 *   1. Buzzer_ON() - 打开蜂鸣器
 *   2. mspm0_delay_ms(DELAY_TIME) - 延时100ms
 *   3. Buzzer_OFF() - 关闭蜂鸣器
 */
void Buzzer_Delay_ON(void)
{
    Buzzer_ON();              /* 打开蜂鸣器 */
    mspm0_delay_ms(DELAY_TIME);  /* 延时100ms */
    Buzzer_OFF();             /* 关闭蜂鸣器 */
}
