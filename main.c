/**
 * @file main.c
 * @brief 主程序源文件
 *
 * 程序架构：
 *   - 系统初始化：时钟、外设、中断
 *   - 主循环：调用菜单状态机
 *
 * 初始化顺序：
 *   1. SYSCFG_DL_init(): 系统配置初始化（芯片级）
 *   2. SysTick_Init(): 系统滴答定时器初始化（延时用）
 *   3. OLED_Init(): OLED显示屏初始化
 *   4. WIT_Init(): 陀螺仪初始化
 *   5. 开启Key定时器中断（按键扫描）
 *   6. 开启蓝牙串口中断
 *   7. Interrupt_Init(): 中断初始化
 *
 * 主循环：
 *   - Menu_StateMachine(): 菜单状态机（阻塞式等待用户选择）
 *   - 各子菜单自行处理按键和任务
 *
 * 中断使用：
 *   - TIMER_Key_INST_IRQHandler: 按键扫描定时器（10ms周期）
 *   - GROUP1_IRQHandler: 编码器外部中断（Menu.c中）
 *   - TIMER_Motor_INST_IRQHandler: 电机PID定时器（Menu.c中）
 *   - TIMER_Grayscale_Sensor_INST_IRQHandler: 灰度传感器定时器（Menu.c中）
 *   - UART_BlueSerial_INST_IRQHandler: 蓝牙串口接收中断（BlueSerial.c中）
 *   - UART_WIT_INST_IRQHandler: 陀螺仪数据接收中断（interrupt.c中）
 */

/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "main.h"

/*================================== 全局变量 =====================================*/
uint8_t oled_buffer[32];   /**< OLED显示缓冲区（备用）*/

/*================================== 主函数 =====================================*/

/**
 * @brief 主函数
 *
 * 程序入口点
 *
 * 初始化流程：
 *   1. 芯片级系统配置
 *   2. 延时定时器初始化
 *   3. OLED显示屏初始化
 *   4. 按键定时器中断初始化
 *   5. 陀螺仪初始化
 *   6. 蓝牙串口中断初始化
 *   7. 通用中断初始化
 *   8. 进入主循环（菜单状态机）
 */
int main(void)
{
    /*-------- 芯片级系统配置 --------*/
    SYSCFG_DL_init();

    /*-------- 延时定时器初始化 --------*/
    SysTick_Init();

    /*-------- OLED显示屏初始化 --------*/
    OLED_Init();

    /*-------- 按键定时器中断初始化 --------*/
    /* 使能定时器零匹配中断（按键扫描）*/
    DL_TimerG_enableInterrupt(TIMER_Key_INST, DL_TIMER_IIDX_ZERO);
    /* 启用NVIC中断通道 */
    NVIC_EnableIRQ(TIMER_Key_INST_INT_IRQN);
    /* 启动按键定时器计数器 */
    DL_TimerG_startCounter(TIMER_Key_INST);

    /*-------- 陀螺仪初始化 --------*/
    WIT_Init();

    /*-------- 蓝牙串口中断初始化 --------*/
    /* 清除挂起的中断标志 */
    NVIC_ClearPendingIRQ(UART_BlueSerial_INST_INT_IRQN);
    /* 启用蓝牙串口接收中断 */
    NVIC_EnableIRQ(UART_BlueSerial_INST_INT_IRQN);

    /*-------- 通用中断初始化 --------*/
    /* Don't remove this! */
    Interrupt_Init();

    /*-------- 主循环 --------*/
    while (1)
    {
        /* 菜单状态机：处理用户界面交互
         * Menu_StateMachine() 会一直阻塞直到用户选择并退出某个子菜单
         * 然后返回此处继续显示主菜单
         */
        Menu_StateMachine();

        /* 下面是一些调试用的代码示例，被注释掉了
         *
         * 显示编码器值和速度：
         * OLED_ShowNum(0, 0, Encoder_L, 8, 16);
         * OLED_ShowNum(0, 2, Encoder_R, 8, 16);
         * OLED_ShowNum(0, 4, Speed_L, 4, 16);
         * OLED_ShowNum(0, 6, Speed_R, 4, 16);
         *
         * 蓝牙打印PID信息：
         * BlueSerial_Printf("L:%f,%d\n", Speed_L, Encoder_L);
         * BlueSerial_Printf("---------- PID Info ----------\n");
         * BlueSerial_Printf("目标速度: %.2f | 实际速度: %.2f\n", 40.0, Speed_L);
         * BlueSerial_Printf("原始误差: %.2f | 滤波误差: %.2f\n", PID_Motor_L.err, PID_Motor_L.prev_filtered_err);
         * BlueSerial_Printf("PID输出:  %.2f | PWM方向: %d | PWM占空比: %.2f\n", PID_Motor_L.output, PWMCommand_L.direction, PWMCommand_L.duty);
         * BlueSerial_Printf("------------------------------\n\n");
         *
         * 灰度传感器调试：
         * Graysccale_Sensor_PinCheck();
         * OLED_ShowNum(0, 0, distance, 3, 16);
         */
    }
}

/*================================== 按键定时器中断处理 =====================================*/

/**
 * @brief 按键定时器中断处理函数
 *
 * 触发条件：TIMER_Key_INST定时器计数达到周期值
 * 触发频率：约10ms一次（由定时器配置决定）
 *
 * 功能说明：
 *   - 调用Key0~Key3的Tick函数扫描按键状态
 *   - 更新各按键的Num标志
 *
 * @note 此中断用于非阻塞式按键检测
 */
void TIMER_Key_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_Key_INST))
    {
        case DL_TIMER_IIDX_ZERO:  /* 定时器零匹配中断 */
            Key0_Tick();  /* 扫描Key0 */
            Key1_Tick();  /* 扫描Key1 */
            Key2_Tick();  /* 扫描Key2 */
            Key3_Tick();  /* 扫描Key3 */
            break;

        default:
            break;
    }
}
