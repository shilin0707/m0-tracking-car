/**
 * @file BlueSerial.c
 * @brief 蓝牙串口通信驱动源文件
 *
 * 功能说明：
 *   - 蓝牙串口数据收发
 *   - 解析蓝牙遥控器发送的摇杆和按键数据
 *   - 计算摇杆控制的目标速度
 *
 * 数据包格式：
 *   - 按键事件：[Key,按键编号,动作(up/down)]
 *   - 滑块事件：[slider,滑块编号,值]
 *   - 摇杆事件：[joystick,LH,LV,RH,RV]
 *
 * 摇杆控制算法：
 *   - LV（左摇杆垂直）：控制前进速度（只能前进）
 *   - RV（右摇杆垂直）：控制转向差值
 *   - 目标速度传递给电机PID控制器
 */

#include "BlueSerial.h"

/*================================== 全局变量 =====================================*/
char BlueSerial_RxPacket[100];     /**< 蓝牙接收数据包缓冲区，格式：[TAG,DATA...] */
uint8_t BlueSerial_RxFlag;         /**< 蓝牙接收完成标志：1=接收到完整数据包，0=无数据 */

/*================================== 函数实现 =====================================*/

/**
 * @brief 蓝牙串口发送一个字节
 *
 * @param Byte 要发送的一个字节数据
 *
 * @note 发送前会等待串口空闲，确保数据不丢失
 */
void BlueSerial_SendByte(uint8_t Byte)
{
    /* 等待串口发送缓冲区空闲 */
    while( DL_UART_isBusy(UART_BlueSerial_INST) == true );
    /* 发送单个字节数据 */
    DL_UART_Main_transmitData(UART_BlueSerial_INST, Byte);
}

/**
 * @brief 蓝牙串口发送一个数组
 *
 * @param Array 要发送数组的首地址
 * @param Length 要发送数组的长度
 *
 * @note 遍历数组，依次发送每个字节
 */
void BlueSerial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)          /* 遍历数组 */
    {
        BlueSerial_SendByte(Array[i]);     /* 发送每个字节 */
    }
}

/**
 * @brief 蓝牙串口发送一个字符串
 *
 * @param String 要发送字符串的首地址
 *
 * @note 遍历字符串，遇到字符串结束标志'\0'时停止
 */
void BlueSerial_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)   /* 遍历字符串直到结束 */
    {
        BlueSerial_SendByte(String[i]);    /* 发送每个字符 */
    }
}

/**
 * @brief 整数次方计算（内部使用）
 *
 * @param X 底数
 * @param Y 指数
 * @return uint32_t 返回 X 的 Y 次方
 *
 * @note 用于 BlueSerial_SendNumber() 中分解数字各位
 */
uint32_t BlueSerial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;      /* 结果初值为1 */
    while (Y--)               /* 执行Y次乘法 */
    {
        Result *= X;          /* 将X累乘到结果 */
    }
    return Result;
}

/**
 * @brief 蓝牙串口发送数字
 *
 * @param Number 要发送的数字（范围：0~4294967295）
 * @param Length 要发送数字的位数（范围：0~10）
 *
 * @note 将数字按位分解，发送每位数字的ASCII码
 *       例如：Number=123, Length=3 -> 发送 '1','2','3'
 */
void BlueSerial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        /* 分解数字的每一位并转换为ASCII字符发送 */
        BlueSerial_SendByte(Number / BlueSerial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
 * @brief 标准库 fputc 重定向
 *
 * @param ch 要输出的字符
 * @param f 文件指针（未使用）
 * @return int 返回输出的字符
 *
 * @note 将标准输出重定向到蓝牙串口，可使用 printf() 输出到蓝牙
 */
int fputc(int ch, FILE *f)
{
    BlueSerial_SendByte(ch);   /* 通过蓝牙串口发送字符 */
    return ch;
}

/**
 * @brief 格式化打印到蓝牙串口
 *
 * @param format 格式化字符串（与printf相同）
 * @param ... 可变参数列表
 *
 * @note 类似于printf，但输出到蓝牙串口而不是标准输出
 *       示例：BlueSerial_Printf("Speed: %d, Angle: %.2f\n", speed, angle);
 */
void BlueSerial_Printf(char *format, ...)
{
    char String[100];          /* 临时字符串缓冲区 */
    va_list arg;               /* 可变参数列表 */
    va_start(arg, format);     /* 初始化可变参数 */
    vsprintf(String, format, arg);  /* 格式化字符串 */
    va_end(arg);               /* 结束可变参数处理 */
    BlueSerial_SendString(String);  /* 发送字符串到蓝牙 */
}

/**
 * @brief 解析蓝牙串口接收到的数据包
 *
 * @param BlueSerial_RxPacket 接收到的数据包缓冲区
 *
 * 功能说明：
 *   - 解析不同类型的蓝牙数据包
 *   - 根据Tag类型分发到相应处理函数
 *
 * 数据包格式：
 *   - 按键事件：[Key,1,up] - 按键1松开
 *   - 滑块事件：[slider,1,500] - 滑块1值为500
 *   - 摇杆事件：[joystick,0,50,0,-30] - LH=0,LV=50,RH=0,RV=-30
 *
 * @note 此函数应在主循环中持续调用
 */
void BlueSerial_Tick(char *BlueSerial_RxPacket)
{
    if(BlueSerial_RxFlag == 1)  /* 检查是否接收到完整数据包 */
    {
        /* 回显收到的数据包（用于调试）*/
        BlueSerial_SendString(BlueSerial_RxPacket);

        /* 使用strtok按逗号分隔数据包 */
        char *Tag = strtok(BlueSerial_RxPacket, ",");

        if(Tag == NULL)
        {
            /* 数据格式错误：无法获取Tag */
        }
        else if(strcmp(Tag, "Key") == 0)
        {
            /*-------- 按键事件处理 --------*/
            char *Name = strtok(NULL, ",");     /* 获取按键编号 */
            char *Action = strtok(NULL, ",");   /* 获取按键动作 */

            if (Name == NULL || Action == NULL)
            {
                /* 数据不完整 */
            }
            else if (strcmp(Name, "1") == 0 && strcmp(Action, "up") == 0)
            {
                /* 按键1松开事件 */
                BlueSerial_SendString((char *)"key,1,up\r\n");
            }
            else if (strcmp(Name, "2") == 0 && strcmp(Action, "down") == 0)
            {
                /* 按键2按下事件 */
                BlueSerial_SendString((char *)"key,2,down\r\n");
            }
        }
        else if (strcmp(Tag, "slider") == 0)
        {
            /*-------- 滑块事件处理 --------*/
            char *Name = strtok(NULL, ",");     /* 获取滑块编号 */
            char *Value = strtok(NULL, ",");    /* 获取滑块值 */

            if (Name == NULL || Value == NULL)
            {
                /* 数据不完整 */
            }
            else if (strcmp(Name, "1") == 0)
            {
                /* 滑块1事件 */
                uint8_t IntValue = atoi(Value);  /* 转换为整数 */
                BlueSerial_SendString((char *)"slider,1:");
                BlueSerial_SendNumber(IntValue, 4);
            }
            else if (strcmp(Name, "2") == 0)
            {
                /* 滑块2事件 */
                float FloatValue = atof(Value);  /* 转换为浮点数 */
                BlueSerial_SendString((char *)"slider,2:");
                BlueSerial_SendNumber((uint32_t)FloatValue, 4);
            }
        }
        else if (strcmp(Tag, "joystick") == 0)
        {
            /*-------- 摇杆事件处理 --------*/
            /* 解析四个摇杆方向值 */
            int8_t LH = atoi(strtok(NULL, ","));  /* 左摇杆水平 */
            int8_t LV = atoi(strtok(NULL, ","));  /* 左摇杆垂直 */
            int8_t RH = atoi(strtok(NULL, ","));  /* 右摇杆水平 */
            int8_t RV = atoi(strtok(NULL, ","));  /* 右摇杆垂直 */

            /* 调用摇杆控制函数，计算目标速度 */
            BlueSerial_Control(LV, RV);
        }

        /* 处理完成后清除接收标志 */
        BlueSerial_RxFlag = 0;
    }
}

/**
 * @brief 摇杆控制函数
 *
 * @param LV 左摇杆垂直方向，范围-100~100，向上为正
 * @param RV 右摇杆垂直方向，范围-100~100，向上为正
 *
 * 控制算法：
 *   - 左摇杆控制基础前进速度（只能前进，不能后退）
 *   - 右摇杆控制转向差值（左正右负）
 *
 * 速度计算：
 *   - 基础速度 = |LV| * MAX_SPEED / 100
 *   - 速度差 = RV * MAX_DIFF / 100
 *   - 左轮目标 = 基础速度 + 速度差
 *   - 右轮目标 = 基础速度 - 速度差
 *
 * @note 此函数设置全局目标速度变量，由电机定时器中断读取并执行PID控制
 */
void BlueSerial_Control(int8_t LV, int8_t RV)
{
    /*-------- 速度参数 --------*/
    #define MAX_SPEED  400      /**< 最大基础速度 */
    #define MAX_DIFF   200      /**< 最大速度差（左右轮速度差异）*/

    /* 设置BlueSerial控制模式标志，供电机定时器中断识别 */
    g_blueserial_control_enable = 1;

    /* 处理死区，防止摇杆在原点附近抖动导致电机抖动 */
    if (LV > -5 && LV < 5)
    {
        LV = 0;  /* 忽略中心5%范围的摇杆值 */
    }
    if (RV > -5 && RV < 5)
    {
        RV = 0;  /* 忽略中心5%范围的摇杆值 */
    }

    /* 取绝对值，简化后续计算（速度只有正方向） */
    if (LV < 0)
    {
        LV = (int8_t)abs(LV);
    }

    float Speed_L, Speed_R;  /* 左右轮临时速度变量 */

    /* 基础速度：左摇杆控制，只能前进
     * LV > 0：摇杆向上，速度为正值
     * LV <= 0：摇杆向下或居中，速度为0
     */
    float BaseSpeed = (LV > 0) ? (LV * MAX_SPEED / 100.0f) : 0;

    /* 速度差：右摇杆控制转向
     * RV > 0：右转（左轮加速，右轮减速）
     * RV < 0：左转（左轮减速，右轮加速）
     * 当基础速度为0时，不计算速度差（避免原地转向）
     */
    float SpeedDiff = (BaseSpeed > 0) ? (RV * MAX_DIFF / 100.0f) : 0;

    /* 左右轮目标速度计算 */
    g_target_speed_L = BaseSpeed + SpeedDiff;  /* 左轮：基础速度 + 差值 */
    g_target_speed_R = BaseSpeed - SpeedDiff;  /* 右轮：基础速度 - 差值 */

    /* 限幅，保证目标速度在有效范围内 */
    if (g_target_speed_L > MAX_SPEED)   g_target_speed_L = MAX_SPEED;  /* 不超过最大速度 */
    if (g_target_speed_L < 0)          g_target_speed_L = 0;          /* 不低于零 */
    if (g_target_speed_R > MAX_SPEED)   g_target_speed_R = MAX_SPEED;
    if (g_target_speed_R < 0)          g_target_speed_R = 0;
}

/**
 * @brief 清除BlueSerial控制模式（摇杆回到原点时调用）
 *
 * 功能说明：
 *   - 清除BlueSerial控制模式标志
 *   - 将目标速度清零
 *
 * @note 当摇杆回到原点时调用此函数停止电机
 */
void BlueSerial_Control_Stop(void)
{
    g_blueserial_control_enable = 0;  /* 清除控制模式标志 */
    g_target_speed_L = 0;             /* 左轮目标速度清零 */
    g_target_speed_R = 0;             /* 右轮目标速度清零 */
}

/**
 * @brief 蓝牙串口接收中断处理函数
 *
 * 触发条件：串口接收到数据（UART接收中断）
 *
 * 状态机说明：
 *   - State 0：等待接收包头'['
 *   - State 1：接收数据包内容，直到遇到包尾']'
 *
 * 数据包格式：[TAG,DATA1,DATA2,...]
 *
 * @note 使用静态变量保持状态，支持连续接收多个数据包
 */
void UART_BlueSerial_INST_IRQHandler(void)
{
    switch( DL_UART_getPendingInterrupt(UART_BlueSerial_INST) )
    {
        case DL_UART_IIDX_RX:  /* 接收中断 */
            {
                /* 读取接收到的数据 */
                uint8_t RxData = DL_UART_Main_receiveData(UART_BlueSerial_INST);

                /* 使用状态机解析数据包 */
                static uint8_t RxState = 0;    /* 当前状态机状态：0=等待包头，1=接收数据 */
                static uint8_t pRxPacket = 0;  /* 当前接收数据的位置索引 */

                /*-------- 状态0：等待接收数据包头 --------*/
                if (RxState == 0)
                {
                    /* 检查是否收到包头'['，并且上一个数据包已处理完毕 */
                    if (RxData == '[' && BlueSerial_RxFlag == 0)
                    {
                        RxState = 1;    /* 切换到数据接收状态 */
                        pRxPacket = 0;  /* 数据包位置归零 */
                    }
                }
                /*-------- 状态1：接收数据包内容 --------*/
                else if (RxState == 1)
                {
                    /* 检查是否收到包尾']' */
                    if (RxData == ']')
                    {
                        /* 接收完成 */
                        RxState = 0;                                      /* 状态归零 */
                        BlueSerial_RxPacket[pRxPacket] = '\0';             /* 添加字符串结束标志 */
                        BlueSerial_RxFlag = 1;                            /* 置位接收完成标志 */
                    }
                    else
                    {
                        /* 正常数据：存入接收缓冲区 */
                        BlueSerial_RxPacket[pRxPacket] = RxData;  /* 存储数据 */
                        pRxPacket++;                              /* 位置索引自增 */
                    }
                }
            }
            break;

        default:  /* 其他串口中断：不处理 */
            break;
    }
}
