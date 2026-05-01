/**
 * @file Key.c
 * @brief 按键驱动源文件
 *
 * 工作原理：
 *   - 定时器中断定期扫描按键状态（通常10ms一次）
 *   - 检测按键按下边沿（电平从高到低）
 *   - 检测到边沿后设置对应Num标志
 *   - Key_GetNum() 返回标志并清除（消费模式）
 *
 * 状态机说明：
 *   - 使用CurrState和PrevState记录当前和上一次按键状态
 *   - 当检测到下降沿（当前=0且上一次!=0）时触发按键事件
 *   - 使用Count计数器实现消抖（每2次扫描判断一次）
 *
 * 注意事项：
 *   - 按键为低电平触发（按下时GPIO读取为0）
 *   - Key_GetNum() 返回1后会清除标志，实现消费模式
 */

#include "Key.h"

/*================================== 全局变量 =====================================*/
/**
 * @brief 按键标志位
 * @note 由定时器中断更新，Key_GetNum()读取并清除
 */
uint8_t Key1_Flag;   /**< Key0的按键标志 */
uint8_t Key2_Flag;   /**< Key1的按键标志 */
uint8_t Key3_Flag;   /**< Key2的按键标志 */
uint8_t Key4_Flag;   /**< Key3的按键标志 */

/**
 * @brief 按键事件标志
 * @note 检测到按键按下边沿时设置为1，Key_GetNum()消费后清零
 */
uint8_t Key1_Num;   /**< Key0的事件标志 */
uint8_t Key2_Num;   /**< Key1的事件标志 */
uint8_t Key3_Num;   /**< Key2的事件标志 */
uint8_t Key4_Num;   /**< Key3的事件标志 */

/*================================== 按键读取函数 =====================================*/
/**
 * @brief 读取Key0状态
 * @return int 按键电平（0=按下，1=松开）
 */
int KeyO_Read()
{
    return DL_GPIO_readPins(GPIO_Key_PORT, GPIO_Key_PIN_Key_0_PIN);
}

/**
 * @brief 读取Key1状态
 * @return int 按键电平（0=按下，1=松开）
 */
int Key1_Read()
{
    return DL_GPIO_readPins(GPIO_Key_PORT, GPIO_Key_PIN_Key_1_PIN);
}

/**
 * @brief 读取Key2状态
 * @return int 按键电平（0=按下，1=松开）
 */
int Key2_Read()
{
    return DL_GPIO_readPins(GPIO_Key_PORT, GPIO_Key_PIN_Key_2_PIN);
}

/**
 * @brief 读取Key3状态
 * @return int 按键电平（0=按下，1=松开）
 */
int Key3_Read()
{
    return DL_GPIO_readPins(GPIO_Key_PORT, GPIO_Key_PIN_Key_3_PIN);
}

/*================================== 按键事件获取函数 =====================================*/
/**
 * @brief 获取Key0按键事件（消费模式）
 * @return uint8_t 返回1表示按键刚按下，返回0表示无事件
 * @note 调用后清除按键事件标志
 */
uint8_t Key0_GetNum(void)
{
    uint8_t Temp;
    if (Key1_Num)  /* 检查Key0事件标志 */
    {
        Temp = Key1_Num;  /* 保存返回值 */
        Key1_Num = 0;     /* 清除标志 */
        return Temp;
    }
    return 0;  /* 无事件 */
}

/**
 * @brief 获取Key1按键事件（消费模式）
 * @return uint8_t 返回1表示按键刚按下，返回0表示无事件
 */
uint8_t Key1_GetNum(void)
{
    uint8_t Temp;
    if (Key2_Num)  /* 检查Key1事件标志 */
    {
        Temp = Key2_Num;
        Key2_Num = 0;
        return Temp;
    }
    return 0;
}

/**
 * @brief 获取Key2按键事件（消费模式）
 * @return uint8_t 返回1表示按键刚按下，返回0表示无事件
 */
uint8_t Key2_GetNum(void)
{
    uint8_t Temp;
    if (Key3_Num)  /* 检查Key2事件标志 */
    {
        Temp = Key3_Num;
        Key3_Num = 0;
        return Temp;
    }
    return 0;
}

/**
 * @brief 获取Key3按键事件（消费模式）
 * @return uint8_t 返回1表示按键刚按下，返回0表示无事件
 */
uint8_t Key3_GetNum(void)
{
    uint8_t Temp;
    if (Key4_Num)  /* 检查Key3事件标志 */
    {
        Temp = Key4_Num;
        Key4_Num = 0;
        return Temp;
    }
    return 0;
}

/*================================== 按键状态获取函数 =====================================*/
/**
 * @brief 获取Key0当前状态
 * @return uint8_t KEY_PRESSED(1)=按下，KEY_UNPRESSED(0)=松开
 */
uint8_t Key0_GetState(void)
{
    if (KeyO_Read() == 0)  /* 低电平表示按下 */
    {
        return KEY_PRESSED;
    }
    return KEY_UNPRESSED;
}

/**
 * @brief 获取Key1当前状态
 * @return uint8_t KEY_PRESSED(1)=按下，KEY_UNPRESSED(0)=松开
 */
uint8_t Key1_GetState(void)
{
    if (Key1_Read() == 0)  /* 低电平表示按下 */
    {
        return KEY_PRESSED;
    }
    return KEY_UNPRESSED;
}

/**
 * @brief 获取Key2当前状态
 * @return uint8_t KEY_PRESSED(1)=按下，KEY_UNPRESSED(0)=松开
 */
uint8_t Key2_GetState(void)
{
    if (Key2_Read() == 0)  /* 低电平表示按下 */
    {
        return KEY_PRESSED;
    }
    return KEY_UNPRESSED;
}

/**
 * @brief 获取Key3当前状态
 * @return uint8_t KEY_PRESSED(1)=按下，KEY_UNPRESSED(0)=松开
 */
uint8_t Key3_GetState(void)
{
    if (Key3_Read() == 0)  /* 低电平表示按下 */
    {
        return KEY_PRESSED;
    }
    return KEY_UNPRESSED;
}

/*================================== 按键定时扫描函数 =====================================*/
/**
 * @brief Key0定时扫描函数
 * @note 由定时器中断（通常10ms）调用，使用Count分频实现消抖
 */
void Key0_Tick(void)
{
    static uint8_t Count1;           /* 扫描分频计数器 */
    static uint8_t CurrState1;       /* 当前状态 */
    static uint8_t PrevState1;       /* 上一次状态 */

    Count1++;  /* 每次中断计数+1 */

    if (Count1 >= 2)  /* 每2次中断（20ms）检测一次 */
    {
        PrevState1 = CurrState1;           /* 保存上次状态 */
        CurrState1 = Key0_GetState();       /* 读取当前状态 */

        /* 检测下降沿（从高电平变为低电平 = 按键按下）*/
        if (CurrState1 == 0 && PrevState1 != 0)
        {
            Key1_Num = PrevState1;  /* 设置按键事件标志 */
        }

        Count1 = 0;  /* 计数器清零 */
    }
}

/**
 * @brief Key1定时扫描函数
 */
void Key1_Tick(void)
{
    static uint8_t Count2;
    static uint8_t CurrState2;
    static uint8_t PrevState2;

    Count2++;

    if (Count2 >= 2)
    {
        PrevState2 = CurrState2;
        CurrState2 = Key1_GetState();

        if (CurrState2 == 0 && PrevState2 != 0)
        {
            Key2_Num = PrevState2;
        }

        Count2 = 0;
    }
}

/**
 * @brief Key2定时扫描函数
 */
void Key2_Tick(void)
{
    static uint8_t Count3;
    static uint8_t CurrState3;
    static uint8_t PrevState3;

    Count3++;

    if (Count3 >= 2)
    {
        PrevState3 = CurrState3;
        CurrState3 = Key2_GetState();

        if (CurrState3 == 0 && PrevState3 != 0)
        {
            Key3_Num = PrevState3;
        }

        Count3 = 0;
    }
}

/**
 * @brief Key3定时扫描函数
 */
void Key3_Tick(void)
{
    static uint8_t Count4;
    static uint8_t CurrState4;
    static uint8_t PrevState4;

    Count4++;

    if (Count4 >= 2)
    {
        PrevState4 = CurrState4;
        CurrState4 = Key3_GetState();

        if (CurrState4 == 0 && PrevState4 != 0)
        {
            Key4_Num = PrevState4;
        }

        Count4 = 0;
    }
}
