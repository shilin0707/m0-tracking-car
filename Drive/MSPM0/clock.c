/**
 * @file clock.c
 * @brief 系统时钟和延时驱动源文件
 *
 * 功能说明：
 *   - 提供毫秒级延时功能
 *   - 提供毫秒级计时功能
 *   - 初始化SysTick定时器
 *
 * SysTick定时器：
 *   - ARM Cortex-M内核提供的定时器
 *   - 每1ms触发一次中断
 *   - 为整个系统提供统一的时间基准
 *
 * 使用说明：
 *   - mspm0_delay_ms(): 阻塞式延时（精确等待指定毫秒）
 *   - mspm0_get_clock_ms(): 获取当前计时值（非阻塞）
 */

#include "clock.h"

/*================================== 全局变量 =====================================*/
volatile unsigned long tick_ms;    /**< 系统毫秒计数（由SysTick中断递增）*/
volatile uint32_t start_time;      /**< 延时起始时间（内部使用）*/

/*================================== 延时函数 =====================================*/

/**
 * @brief 毫秒级延时函数
 *
 * @param num_ms 要延时的毫秒数
 * @return int 0=成功
 *
 * 实现原理：
 *   - 记录当前tick_ms值作为起始时间
 *   - 等待tick_ms与起始时间的差达到num_ms
 *   - 由于SysTick每1ms中断一次，此为阻塞式延时
 *
 * @note 这是阻塞式延时，期间CPU不能处理其他任务
 */
int mspm0_delay_ms(unsigned long num_ms)
{
    start_time = tick_ms;  /* 记录起始时间 */
    while (tick_ms - start_time < num_ms)  /* 等待达到目标延时 */
        ;  /* 空循环 */
    return 0;
}

/**
 * @brief 获取当前毫秒计时值
 *
 * @param count 返回当前tick_ms值的指针
 * @return int 0=成功，1=参数错误
 *
 * @note 这是非阻塞查询，适合需要知道当前时间的场景
 */
int mspm0_get_clock_ms(unsigned long *count)
{
    if (!count)  /* 参数检查 */
        return 1;
    count[0] = tick_ms;  /* 返回当前计时值 */
    return 0;
}

/*================================== SysTick初始化 =====================================*/

/**
 * @brief SysTick定时器初始化
 *
 * 功能说明：
 *   - 配置SysTick定时器每1ms触发一次中断
 *   - 设置最高优先级
 *   - 启用中断和计数器
 *
 * 配置计算：
 *   - CPU频率：CPUCLK_FREQ（Hz）
 *   - 分频系数：CPUCLK_FREQ/1000 = 每毫秒计数次数
 *   - 例如：40MHz / 1000 = 40000，即每1ms计数40000下
 */
void SysTick_Init(void)
{
    /* 配置SysTick：时钟源=CPU时钟，周期=CPUCLK_FREQ/1000（1ms）*/
    DL_SYSTICK_config(CPUCLK_FREQ / 1000);

    /* 设置SysTick中断优先级为0（最高优先级）*/
    NVIC_SetPriority(SysTick_IRQn, 0);
}
