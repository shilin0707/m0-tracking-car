/**
 * @file Encoder.c
 * @brief 编码器驱动源文件
 *
 * AB相编码器原理：
 *   - A相和B相输出相位差90°的方波
 *   - 通过A相的边沿触发中断，在中断中读取B相电平判断方向
 *   - A相上升沿时：
 *     - 如果B相为高电平，表示正转
 *     - 如果B相为低电平，表示反转
 *
 * 硬件连接：
 *   - 左编码器：Left_A, Left_B
 *   - 右编码器：Right_A, Right_B
 *
 * 注意事项：
 *   - 左右轮是对着装的，所以前进时一个正转一个反转
 *   - 使用int32_t类型避免溢出
 */

#include "Encoder.h"

/*================================== 全局变量 =====================================*/
/**
 * @brief 编码器累计计数值
 * @note 由 Encoder_Check() 在外部中断中更新
 *       需要用 int32_t 类型以支持负数计数
 */
int32_t Encoder_L;   /**< 左编码器累计脉冲数 */
int32_t Encoder_R;   /**< 右编码器累计脉冲数 */

/*================================== 引脚读取函数 =====================================*/
/**
 * @brief 读取左编码器A相信号
 * @return int 返回引脚电平状态（0或1）
 */
int Read_Left_A(void)
{
    return DL_GPIO_readPins(GPIO_Encoder_PORT, GPIO_Encoder_Left_A_PIN);
}

/**
 * @brief 读取左编码器B相信号
 * @return int 返回引脚电平状态（0或1）
 */
int Read_Left_B(void)
{
    return DL_GPIO_readPins(GPIO_Encoder_PORT, GPIO_Encoder_Left_B_PIN);
}

/**
 * @brief 读取右编码器A相信号
 * @return int 返回引脚电平状态（0或1）
 */
int Read_Right_A(void)
{
    return DL_GPIO_readPins(GPIO_Encoder_PORT, GPIO_Encoder_Right_A_PIN);
}

/**
 * @brief 读取右编码器B相信号
 * @return int 返回引脚电平状态（0或1）
 */
int Read_Right_B(void)
{
    return DL_GPIO_readPins(GPIO_Encoder_PORT, GPIO_Encoder_Right_B_PIN);
}

/*================================== 编码器计数处理 =====================================*/

/**
 * @brief 编码器计数处理函数
 *
 * 功能说明：
 *   - 检测四个编码器引脚（左右编码器的A和B相）
 *   - 根据AB相的电平关系判断转动方向
 *   - 更新Encoder_L和Encoder_R计数值
 *   - 清除已处理的中断标志
 *
 * 转向判断逻辑：
 *   - 左轮（对着装）：前进时反转，计数减小
 *     - A相上升沿时B相为低 -> 计数减（反转）
 *     - A相上升沿时B相为高 -> 计数加（正转）
 *   - 右轮（对着装）：前进时正转，计数增大
 *     - A相上升沿时B相为低 -> 计数加（正转）
 *     - A相上升沿时B相为高 -> 计数减（反转）
 *
 * 中断触发条件：
 *   - 左轮A相引脚中断
 *   - 左轮B相引脚中断
 *   - 右轮A相引脚中断
 *   - 右轮B相引脚中断
 *
 * @note 此函数由GPIO外部中断（GROUP1_IRQHandler）调用
 */
void Encoder_Check(void)
{
    /* 说明：两个轮子是对着放的，所以小车向前走（轮子顺时针转）时，
     * 一定会是一个正转一个反转（计数值一增一减）
     */

    /*-------- 左轮A相触发中断 --------*/
    if (DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_Left_A_PIN))
    {
        /* A相上升沿时，判断B相电平确定方向 */
        if (Read_Left_B() == 0)
        {
            Encoder_L--;  /* B相低电平 -> 反转（前进时左轮反转）*/
        }
        else
        {
            Encoder_L++;  /* B相高电平 -> 正转 */
        }
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_Left_A_PIN);  /* 清除中断标志 */
    }

    /*-------- 左轮B相触发中断 --------*/
    if (DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_Left_B_PIN))
    {
        if (Read_Left_A() == 0)
        {
            Encoder_L--;  /* A相低电平 -> 反转 */
        }
        else
        {
            Encoder_L++;  /* A相高电平 -> 正转 */
        }
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_Left_B_PIN);
    }

    /*-------- 右轮A相触发中断 --------*/
    if (DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_Right_A_PIN))
    {
        /* 右轮前进时正转，与左轮相反 */
        if (Read_Right_B() == 0)
        {
            Encoder_R++;  /* B相低电平 -> 正转（前进时右轮正转）*/
        }
        else
        {
            Encoder_R--;  /* B相高电平 -> 反转 */
        }
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_Right_A_PIN);
    }

    /*-------- 右轮B相触发中断 --------*/
    if (DL_GPIO_getEnabledInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_Right_B_PIN))
    {
        if (Read_Right_A() == 0)
        {
            Encoder_R++;  /* A相低电平 -> 正转 */
        }
        else
        {
            Encoder_R--;  /* A相高电平 -> 反转 */
        }
        DL_GPIO_clearInterruptStatus(GPIO_Encoder_PORT, GPIO_Encoder_Right_B_PIN);
    }
}
