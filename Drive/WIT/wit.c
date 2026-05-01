/**
 * @file wit.c
 * @brief 陀螺仪（WIT IMU）驱动源文件
 *
 * 通信方式：
 *   - 串口UART接收陀螺仪数据
 *   - 使用DMA自动接收数据，减轻CPU负担
 *   - 数据包格式：[Header, Data0, Data1, ..., Data31, Checksum]
 *
 * SysConfig配置说明：
 *   - UART波特率：根据模块配置设置
 *   - 通信方向：仅接收（RX only）
 *   - 启用FIFO和RX超时中断
 *   - DMA触发源：UART RX中断
 *   - 传输长度：32字节
 *   - 地址模式：固定源地址（UART数据寄存器）到块目标地址（缓冲区）
 */

#include "wit.h"

 /*================================== 全局变量 =====================================*/
/**
 * @brief DMA接收缓冲区
 * @note 存放从UART接收的陀螺仪原始数据
 */
uint8_t wit_dmaBuffer[33];

/**
 * @brief 陀螺仪数据结构
 * @note 存放解析后的陀螺仪数据
 */
WIT_Data_t wit_data;

/*================================== 初始化函数 =====================================*/

/**
 * @brief 陀螺仪初始化
 *
 * 功能说明：
 *   - 配置DMA通道，从UART接收数据到缓冲区
 *   - 设置DMA源地址：UART数据寄存器
 *   - 设置DMA目标地址：wit_dmaBuffer缓冲区
 *   - 设置传输长度：32字节
 *   - 启用DMA通道和UART中断
 *
 * @note 调用此函数后，陀螺仪数据会自动通过DMA接收
 */
void WIT_Init(void)
{
    /* 设置DMA源地址：UART接收数据寄存器 */
    DL_DMA_setSrcAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t)(&UART_WIT_INST->RXDATA));

    /* 设置DMA目标地址：接收缓冲区 */
    DL_DMA_setDestAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t)&wit_dmaBuffer[0]);

    /* 设置传输长度：32字节 */
    DL_DMA_setTransferSize(DMA, DMA_WIT_CHAN_ID, 32);

    /* 启用DMA通道 */
    DL_DMA_enableChannel(DMA, DMA_WIT_CHAN_ID);

    /* 启用UART接收中断 */
    NVIC_EnableIRQ(UART_WIT_INST_INT_IRQN);
}
