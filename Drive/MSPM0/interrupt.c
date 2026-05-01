/**
 * @file interrupt.c
 * @brief 中断处理源文件
 *
 * 本文件包含以下中断处理：
 *   - SysTick_Handler: 系统滴答定时器中断（用于延时和计时）
 *   - UART_WIT_INST_IRQHandler: 陀螺仪UART接收中断
 *   - UART_BNO08X_INST_IRQHandler: BNO080 IMU UART接收中断（条件编译）
 *
 * 中断优先级：
 *   - SysTick优先级最高（0）
 *
 * 注意：
 *   - GROUP1_IRQHandler已在Menu.c中实现，用于编码器中断
 */

#include "interrupt.h"

/*================================== 全局变量 =====================================*/
uint8_t enable_group1_irq = 0;  /**< GROUP1中断使能标志 */

/*================================== 中断初始化 =====================================*/

/**
 * @brief 中断初始化函数
 *
 * @note 目前仅用于条件性地启用GROUP1中断
 */
void Interrupt_Init(void)
{
    if (enable_group1_irq)
    {
        NVIC_EnableIRQ(1);  /* 启用GROUP1中断 */
    }
}

/*================================== SysTick定时器中断 =====================================*/

/**
 * @brief SysTick定时器中断处理函数
 *
 * 功能说明：
 *   - 每1ms触发一次（由SysTick_Init配置）
 *   - tick_ms全局变量递增，用于延时和计时功能
 *
 * @note 这是最基础的中断，为其他模块提供时间基准
 */
void SysTick_Handler(void)
{
    tick_ms++;  /* 毫秒计数递增 */
}

#if defined UART_BNO08X_INST_IRQHandler
/*================================== BNO080 IMU中断处理 =====================================*/

/**
 * @brief BNO080 IMU串口接收中断处理函数
 *
 * 数据包格式：
 *   - 帧头：0xAA 0xAA（2字节）
 *   - 数据：14字节
 *   - 校验和：1字节
 *   - 总计：19字节
 *
 * 数据内容：
 *   - 索引、偏航角、俯仰角、横滚角
 *   - 三轴加速度
 */
void UART_BNO08X_INST_IRQHandler(void)
{
    uint8_t checkSum = 0;
    extern uint8_t bno08x_dmaBuffer[19];

    /* 禁止DMA通道，获取已接收数据长度 */
    DL_DMA_disableChannel(DMA, DMA_BNO08X_CHAN_ID);
    uint8_t rxSize = 18 - DL_DMA_getTransferSize(DMA, DMA_BNO08X_CHAN_ID);

    /* 读取FIFO中剩余数据 */
    if (DL_UART_isRXFIFOEmpty(UART_BNO08X_INST) == false)
        bno08x_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_BNO08X_INST);

    /* 计算校验和 */
    for (int i = 2; i <= 14; i++)
        checkSum += bno08x_dmaBuffer[i];

    /* 验证数据包的完整性 */
    if ((rxSize == 19) && (bno08x_dmaBuffer[0] == 0xAA) &&
        (bno08x_dmaBuffer[1] == 0xAA) && (checkSum == bno08x_dmaBuffer[18]))
    {
        /* 解析数据包 */
        bno08x_data.index = bno08x_dmaBuffer[2];
        bno08x_data.yaw = (int16_t)((bno08x_dmaBuffer[4] << 8) | bno08x_dmaBuffer[3]) / 100.0;
        bno08x_data.pitch = (int16_t)((bno08x_dmaBuffer[6] << 8) | bno08x_dmaBuffer[5]) / 100.0;
        bno08x_data.roll = (int16_t)((bno08x_dmaBuffer[8] << 8) | bno08x_dmaBuffer[7]) / 100.0;
        bno08x_data.ax = (bno08x_dmaBuffer[10] << 8) | bno08x_dmaBuffer[9];
        bno08x_data.ay = (bno08x_dmaBuffer[12] << 8) | bno08x_dmaBuffer[11];
        bno08x_data.az = (bno08x_dmaBuffer[14] << 8) | bno08x_dmaBuffer[13];
    }

    /* 清空FIFO */
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_BNO08X_INST, dummy, 4);

    /* 重新配置DMA，准备下一次接收 */
    DL_DMA_setDestAddr(DMA, DMA_BNO08X_CHAN_ID, (uint32_t)&bno08x_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_BNO08X_CHAN_ID, 18);
    DL_DMA_enableChannel(DMA, DMA_BNO08X_CHAN_ID);
}
#endif

#if defined UART_WIT_INST_IRQHandler
/*================================== WIT IMU中断处理 =====================================*/

/**
 * @brief WIT IMU串口接收中断处理函数
 *
 * 数据包格式：
 *   - 帧头：0x55（1字节）
 *   - 数据类型：1字节
 *   - 数据：9字节
 *   - 校验和：1字节
 *   - 总计：11字节
 *
 * 支持的数据类型：
 *   - 0x51：加速度数据（ax, ay, az, 温度）
 *   - 0x52：角速度数据（gx, gy, gz）
 *   - 0x53：角度数据（roll, pitch, yaw）
 */
void UART_WIT_INST_IRQHandler(void)
{
    uint8_t checkSum, packCnt = 0;
    extern uint8_t wit_dmaBuffer[33];

    /* 禁止DMA通道，获取已接收数据长度 */
    DL_DMA_disableChannel(DMA, DMA_WIT_CHAN_ID);
    uint8_t rxSize = 32 - DL_DMA_getTransferSize(DMA, DMA_WIT_CHAN_ID);

    /* 读取FIFO中剩余数据 */
    if (DL_UART_isRXFIFOEmpty(UART_WIT_INST) == false)
        wit_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_WIT_INST);

    /* 循环解析所有完整的数据包（每包11字节）*/
    while (rxSize >= 11)
    {
        checkSum = 0;

        /* 计算校验和（不包括校验位）*/
        for (int i = packCnt * 11; i < (packCnt + 1) * 11 - 1; i++)
            checkSum += wit_dmaBuffer[i];

        /* 验证数据包完整性 */
        if ((wit_dmaBuffer[packCnt * 11] == 0x55) &&
            (checkSum == wit_dmaBuffer[packCnt * 11 + 10]))
        {
            /*-------- 0x51: 加速度数据 --------*/
            if (wit_dmaBuffer[packCnt * 11 + 1] == 0x51)
            {
                wit_data.ax = (int16_t)((wit_dmaBuffer[packCnt * 11 + 3] << 8) |
                                         wit_dmaBuffer[packCnt * 11 + 2]) / 2.048;  /* mg */
                wit_data.ay = (int16_t)((wit_dmaBuffer[packCnt * 11 + 5] << 8) |
                                         wit_dmaBuffer[packCnt * 11 + 4]) / 2.048;  /* mg */
                wit_data.az = (int16_t)((wit_dmaBuffer[packCnt * 11 + 7] << 8) |
                                         wit_dmaBuffer[packCnt * 11 + 6]) / 2.048;  /* mg */
                wit_data.temperature = (int16_t)((wit_dmaBuffer[packCnt * 11 + 9] << 8) |
                                                 wit_dmaBuffer[packCnt * 11 + 8]) / 100.0;  /* °C */
            }
            /*-------- 0x52: 角速度数据 --------*/
            else if (wit_dmaBuffer[packCnt * 11 + 1] == 0x52)
            {
                wit_data.gx = (int16_t)((wit_dmaBuffer[packCnt * 11 + 3] << 8) |
                                         wit_dmaBuffer[packCnt * 11 + 2]) / 16.384;  /* °/S */
                wit_data.gy = (int16_t)((wit_dmaBuffer[packCnt * 11 + 5] << 8) |
                                         wit_dmaBuffer[packCnt * 11 + 4]) / 16.384;  /* °/S */
                wit_data.gz = (int16_t)((wit_dmaBuffer[packCnt * 11 + 7] << 8) |
                                         wit_dmaBuffer[packCnt * 11 + 6]) / 16.384;  /* °/S */
            }
            /*-------- 0x53: 角度数据 --------*/
            else if (wit_dmaBuffer[packCnt * 11 + 1] == 0x53)
            {
                wit_data.roll = (int16_t)((wit_dmaBuffer[packCnt * 11 + 3] << 8) |
                                           wit_dmaBuffer[packCnt * 11 + 2]) /
                                 32768.0 * 180.0;  /* ° */
                wit_data.pitch = (int16_t)((wit_dmaBuffer[packCnt * 11 + 5] << 8) |
                                            wit_dmaBuffer[packCnt * 11 + 4]) /
                                 32768.0 * 180.0;  /* ° */
                wit_data.yaw = (int16_t)((wit_dmaBuffer[packCnt * 11 + 7] << 8) |
                                          wit_dmaBuffer[packCnt * 11 + 6]) /
                               32768.0 * 180.0;  /* ° */
                wit_data.version = (int16_t)((wit_dmaBuffer[packCnt * 11 + 9] << 8) |
                                              wit_dmaBuffer[packCnt * 11 + 8]);
            }
        }

        rxSize -= 11;  /* 处理下一个数据包 */
        packCnt++;
    }

    /* 清空FIFO */
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_WIT_INST, dummy, 4);

    /* 重新配置DMA，准备下一次接收 */
    DL_DMA_setDestAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t)&wit_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_WIT_CHAN_ID, 32);
    DL_DMA_enableChannel(DMA, DMA_WIT_CHAN_ID);
}
#endif

/*================================== 注释掉的代码 =====================================*/
/*
 * 以下是备用的GROUP1_IRQHandler实现，当前使用Menu.c中的版本
 */

// void GROUP1_IRQHandler(void)
// {
//     switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) {
//         case GPIO_MULTIPLE_GPIOA_INT_IIDX:
//             /* 处理GPIOA中断 */
//             break;
//         case GPIO_MULTIPLE_GPIOB_INT_IIDX:
//             /* 处理GPIOB中断 */
//             break;
//     }
// }
