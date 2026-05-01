/**
 * @file oled_software_i2c.c
 * @brief OLED显示屏软件I2C驱动源文件
 *
 * SSD1306 OLED控制器说明：
 *   - 分辨率：128x64像素
 *   - 显存组织：8页(Page)，每页128列，列高位低地址
 *   - I2C地址：0x78（7位地址为0x3C）
 *   - 通信协议：I2C从机模式
 *
 * 显存结构（GDDRAM）：
 *   - 将128x64像素分为8个8像素高的"页"
 *   - 每页有128列，每列8像素（1字节数据）
 *   - 字节的bit7对应该列的最上面一个像素，bit0对应最下面一个像素
 *
 * 坐标系统：
 *   - X坐标：0~127（列）
 *   - Y坐标：0~63（行/像素位置）
 *   - 实际Y坐标需要转换为页地址：Page = Y / 8
 */

#include "oled_software_i2c.h"

/*================================== OLED显存结构说明 =====================================*/
/**
 * OLED显存存放格式（128列 x 64行，分为8页）：
 *
 * Page 0: [0]0 1 2 3 ... 127  (行0~7的像素)
 * Page 1: [1]0 1 2 3 ... 127  (行8~15的像素)
 * Page 2: [2]0 1 2 3 ... 127  (行16~23的像素)
 * Page 3: [3]0 1 2 3 ... 127  (行24~31的像素)
 * Page 4: [4]0 1 2 3 ... 127  (行32~39的像素)
 * Page 5: [5]0 1 2 3 ... 127  (行40~47的像素)
 * Page 6: [6]0 1 2 3 ... 127  (行48~55的像素)
 * Page 7: [7]0 1 2 3 ... 127  (行56~63的像素)
 */

/*================================== 延时函数 =====================================*/

/**
 * @brief 毫秒延时函数
 * @param ms 延时时间（毫秒）
 */
void delay_ms(uint32_t ms)
{
    mspm0_delay_ms(ms);
}

/*================================== 显示效果设置 =====================================*/

/**
 * @brief OLED颜色反转
 * @param i 0=正常显示，1=反色显示
 *
 * 正常显示：背景黑，前景亮
 * 反色显示：背景亮，前景黑
 */
void OLED_ColorTurn(uint8_t i)
{
    if (i == 0)
    {
        OLED_WR_Byte(0xA6, OLED_CMD);  /* 正常显示 */
    }
    if (i == 1)
    {
        OLED_WR_Byte(0xA7, OLED_CMD);  /* 反色显示 */
    }
}

/**
 * @brief OLED显示方向旋转180度
 * @param i 0=正常显示，1=180度翻转
 *
 * 用于屏幕装反的情况，或需要特定方向显示
 */
void OLED_DisplayTurn(uint8_t i)
{
    if (i == 0)
    {
        OLED_WR_Byte(0xC8, OLED_CMD);  /* 正常扫描方向 */
        OLED_WR_Byte(0xA1, OLED_CMD);  /* 列地址0在左侧 */
    }
    if (i == 1)
    {
        OLED_WR_Byte(0xC0, OLED_CMD);  /* 反转扫描方向 */
        OLED_WR_Byte(0xA0, OLED_CMD);  /* 列地址0在右侧 */
    }
}

/*================================== 软件I2C时序函数 =====================================*/

/**
 * @brief I2C起始信号
 *
 * I2C起始条件：SCL高电平时，SDA从高变低
 */
void I2C_Start(void)
{
    OLED_SDA_Set();   /* SDA高电平 */
    OLED_SCL_Set();   /* SCL高电平 */

    OLED_SDA_Clr();   /* SDA低电平（起始条件）*/
    OLED_SCL_Clr();   /* SCL低电平（开始传输）*/
}

/**
 * @brief I2C停止信号
 *
 * I2C停止条件：SCL高电平时，SDA从低变高
 */
void I2C_Stop(void)
{
    OLED_SDA_Clr();   /* SDA低电平 */
    OLED_SCL_Set();   /* SCL高电平 */

    OLED_SDA_Set();   /* SDA高电平（停止条件）*/
}

/**
 * @brief I2C等待从机应答
 *
 * 主机释放SDA线后，从机应拉低SDA表示应答
 */
void I2C_WaitAck(void)
{
    OLED_SDA_Set();   /* 主机释放SDA线 */

    OLED_SCL_Set();   /* SCL高电平（读取应答）*/

    OLED_SCL_Clr();   /* SCL低电平 */
}

/**
 * @brief I2C发送一个字节
 *
 * @param dat 要发送的字节数据
 *
 * 发送顺序：从最高位(bit7)到最低位(bit0)
 */
void Send_Byte(uint8_t dat)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        OLED_SCL_Clr();   /* SCL低电平，准备发送下一位 */

        /* 发送数据位 */
        if (dat & 0x80)  /* 判断最高位是否为1 */
        {
            OLED_SDA_Set();  /* bit7=1，发送高电平 */
        }
        else
        {
            OLED_SDA_Clr();  /* bit7=0，发送低电平 */
        }

        OLED_SCL_Set();   /* SCL高电平，数据有效 */
        OLED_SCL_Clr();   /* SCL低电平，准备下一位 */

        dat <<= 1;        /* 左移一位，发送下一位 */
    }
}

/*================================== OLED读写函数 =====================================*/

/**
 * @brief 向SSD1306写入一个字节
 *
 * @param dat 要写入的数据
 * @param mode 传输类型：OLED_CMD=命令，OLED_DATA=数据
 *
 * I2C传输格式：
 *   [Start] [0x78] [ACK] [0x40/0x00] [ACK] [dat] [ACK] [Stop]
 *          ^设备地址    ^数据/命令  ^数据
 */
void OLED_WR_Byte(uint8_t dat, uint8_t mode)
{
    I2C_Start();           /* 起始信号 */
    Send_Byte(0x78);       /* 发送设备地址（7位地址0x3C + 读写位0）*/
    I2C_WaitAck();         /* 等待应答 */

    /* 发送数据类型 */
    if (mode)
    {
        Send_Byte(0x40);   /* 数据传输 */
    }
    else
    {
        Send_Byte(0x00);   /* 命令传输 */
    }
    I2C_WaitAck();

    Send_Byte(dat);        /* 发送数据 */
    I2C_WaitAck();
    I2C_Stop();            /* 停止信号 */
}

/*================================== 显示控制函数 =====================================*/

/**
 * @brief 设置OLED光标位置
 *
 * @param x 列地址（0~127）
 * @param y 页地址（0~7）
 *
 * SSD1306命令格式：
 *   - 0xB0 + y：设置页地址
 *   - ((x & 0xF0) >> 4) | 0x10：设置列高位
 *   - (x & 0x0F)：设置列低位
 */
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
    OLED_WR_Byte(0xB0 + y, OLED_CMD);                     /* 设置页地址 */
    OLED_WR_Byte(((x & 0xF0) >> 4) | 0x10, OLED_CMD);    /* 设置列高位 */
    OLED_WR_Byte((x & 0x0F), OLED_CMD);                   /* 设置列低位 */
}

/**
 * @brief 开启OLED显示
 */
void OLED_Display_On(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);  /* SET DCDC命令 */
    OLED_WR_Byte(0x14, OLED_CMD);  /* DCDC ON */
    OLED_WR_Byte(0xAF, OLED_CMD);  /* DISPLAY ON */
}

/**
 * @brief 关闭OLED显示
 */
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);  /* SET DCDC命令 */
    OLED_WR_Byte(0x10, OLED_CMD);  /* DCDC OFF */
    OLED_WR_Byte(0xAE, OLED_CMD);  /* DISPLAY OFF */
}

/**
 * @brief OLED清屏
 *
 * 将所有像素关闭，显示全黑（与点亮效果相同）
 *
 * 清屏方式：遍历所有8页，每页写入128字节0
 */
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xB0 + i, OLED_CMD);   /* 设置页地址（0~7）*/
        OLED_WR_Byte(0x00, OLED_CMD);       /* 设置列低地址 */
        OLED_WR_Byte(0x10, OLED_CMD);       /* 设置列高地址 */
        for (n = 0; n < 128; n++)
        {
            OLED_WR_Byte(0, OLED_DATA);     /* 写入0x00 */
        }
    }
}

/*================================== 字符显示函数 =====================================*/

/**
 * @brief 在指定位置显示一个字符
 *
 * @param x 列位置（0~127）
 * @param y 行位置（0~63）
 * @param chr 要显示的字符
 * @param sizey 字体大小（8=8x16，16=16x16等）
 *
 * 支持字体：
 *   - 6x8字体
 *   - 8x16字体
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t sizey)
{
    uint8_t c = 0, sizex = sizey / 2;  /* sizex=字符宽度 */
    uint16_t i = 0, size1;
    if (sizey == 8)
        size1 = 6;  /* 6x8字体实际宽度为6 */
    else
        size1 = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * (sizey / 2);  /* 计算字符字节数 */

    c = chr - ' ';  /* ASCII字符偏移（' '的ASCII是32）*/
    OLED_Set_Pos(x, y);  /* 设置光标位置 */

    for (i = 0; i < size1; i++)
    {
        if (i % sizex == 0 && sizey != 8)
            OLED_Set_Pos(x, y++);  /* 换行处理 */

        if (sizey == 8)
            OLED_WR_Byte(asc2_0806[c][i], OLED_DATA);  /* 6x8字体 */
        else if (sizey == 16)
            OLED_WR_Byte(asc2_1608[c][i], OLED_DATA);  /* 8x16字体 */
        else
            return;  /* 不支持的字体大小 */
    }
}

/**
 * @brief 整数次方计算
 * @param m 底数
 * @param n 指数
 * @return uint32_t m的n次方
 */
uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}

/**
 * @brief OLED显示数字
 *
 * @param x 列位置（0~127）
 * @param y 行位置（0~63）
 * @param num 要显示的数字（无符号32位）
 * @param len 数字的位数
 * @param sizey 字体大小
 *
 * 特点：自动补零（数字位数不足时前面补空格）
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t sizey)
{
    uint8_t t, temp, m = 0;
    uint8_t enshow = 0;
    if (sizey == 8)
        m = 2;  /* 8像素字体的宽度补偿 */

    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;  /* 分离出每一位数字 */

        /* 补零处理：前面几位为0时不显示 */
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (sizey / 2 + m) * t, y, ' ', sizey);  /* 显示空格 */
                continue;
            }
            else
                enshow = 1;  /* 遇到非零数字后不再补零 */
        }
        OLED_ShowChar(x + (sizey / 2 + m) * t, y, temp + '0', sizey);  /* 显示数字字符 */
    }
}

/**
 * @brief OLED显示字符串
 *
 * @param x 列位置（0~127）
 * @param y 行位置（0~63）
 * @param chr 字符串指针
 * @param sizey 字体大小
 */
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t sizey)
{
    uint8_t j = 0;
    while (chr[j] != '\0')  /* 遍历字符串直到结束 */
    {
        OLED_ShowChar(x, y, chr[j++], sizey);  /* 显示单个字符 */

        /* 根据字体大小调整列间距 */
        if (sizey == 8)
            x += 6;    /* 6x8字体宽度6 */
        else
            x += sizey / 2;  /* 其他字体宽度 */
    }
}

/**
 * @brief OLED显示汉字
 *
 * @param x 列位置（0~127）
 * @param y 行位置（0~63）
 * @param no 汉字在字库数组中的索引
 * @param sizey 字体大小（目前支持16）
 */
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t no, uint8_t sizey)
{
    uint16_t i, size1 = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    for (i = 0; i < size1; i++)
    {
        if (i % sizey == 0)
            OLED_Set_Pos(x, y++);
        if (sizey == 16)
            OLED_WR_Byte(Hzk[no][i], OLED_DATA);  /* 16x16汉字 */
        else
            return;
    }
}

/*================================== 图片显示函数 =====================================*/

/**
 * @brief OLED显示图片
 *
 * @param x 起始列位置
 * @param y 起始行位置
 * @param sizex 图片宽度（像素）
 * @param sizey 图片高度（像素）
 * @param BMP 图片数据数组（按行存储）
 *
 * 图片格式：单色位图，每字节8个像素（高位在前）
 */
void OLED_DrawBMP(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, uint8_t BMP[])
{
    uint16_t j = 0;
    uint8_t i, m;
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);  /* 转换为页数 */

    for (i = 0; i < sizey; i++)
    {
        OLED_Set_Pos(x, i + y);  /* 设置起始页 */
        for (m = 0; m < sizex; m++)
        {
            OLED_WR_Byte(BMP[j++], OLED_DATA);  /* 写入像素数据 */
        }
    }
}

/*================================== 初始化函数 =====================================*/

/**
 * @brief SSD1306 OLED初始化
 *
 * 初始化序列（参考SSD1306数据手册）：
 *   1. 关闭显示 (0xAE)
 *   2. 设置时钟分频 (0xD5)
 *   3. 设置 multiplex ratio (0xA8)
 *   4. 设置显示偏移 (0xD3)
 *   5. 设置起始行 (0x40)
 *   6. 设置电源管理 (0x8D)
 *   7. 设置显存寻址模式 (0x20)
 *   8. 设置 SEG/COM 引脚配置 (0xDA)
 *   9. 设置 COM 扫描方向 (0xC8)
 *   10. 设置 SEG 列映射 (0xA1)
 *   11. 设置对比度 (0x81)
 *   12. 关闭全部显示 (0xA4)
 *   13. 设置显示极性 (0xA6)
 *   14. 清屏
 *   15. 开启显示 (0xAF)
 */
void OLED_Init(void)
{
    delay_ms(200);  /* 等待OLED上电稳定 */

    OLED_WR_Byte(0xAE, OLED_CMD);  /* 关闭显示 */
    OLED_WR_Byte(0x00, OLED_CMD);  /* 设置低列地址 */
    OLED_WR_Byte(0x10, OLED_CMD);  /* 设置高列地址 */
    OLED_WR_Byte(0x40, OLED_CMD);  /* 设置起始行地址 */
    OLED_WR_Byte(0x81, OLED_CMD);  /* 设置对比度 */
    OLED_WR_Byte(0xCF, OLED_CMD);  /* 对比度值 */
    OLED_WR_Byte(0xA1, OLED_CMD);  /* 设置列映射（0xA0=正常，0xA1=翻转）*/
    OLED_WR_Byte(0xC8, OLED_CMD);  /* 设置扫描方向（0xC0=翻转，0xC8=正常）*/
    OLED_WR_Byte(0xA6, OLED_CMD);  /* 设置正常/反色显示 */
    OLED_WR_Byte(0xA8, OLED_CMD);  /* 设置 multiplex ratio */
    OLED_WR_Byte(0x3F, OLED_CMD);  /* 1/64 duty */
    OLED_WR_Byte(0xD3, OLED_CMD);  /* 设置显示偏移 */
    OLED_WR_Byte(0x00, OLED_CMD);  /* 无偏移 */
    OLED_WR_Byte(0xD5, OLED_CMD);  /* 设置显示时钟 */
    OLED_WR_Byte(0x80, OLED_CMD);  /* 时钟频率 */
    OLED_WR_Byte(0xD9, OLED_CMD);  /* 设置预充电周期 */
    OLED_WR_Byte(0xF1, OLED_CMD);  /* 预充电15 clocks，放电1 clock */
    OLED_WR_Byte(0xDA, OLED_CMD);  /* 设置COM引脚硬件配置 */
    OLED_WR_Byte(0x12, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD);  /* 设置 VCOMH */
    OLED_WR_Byte(0x40, OLED_CMD);  /* VCOM Deselect Level */
    OLED_WR_Byte(0x20, OLED_CMD);  /* 设置显存寻址模式 */
    OLED_WR_Byte(0x02, OLED_CMD);  /* 页寻址模式 */
    OLED_WR_Byte(0x8D, OLED_CMD);  /* 设置 Charge Pump */
    OLED_WR_Byte(0x14, OLED_CMD);  /* Charge Pump 开启 */
    OLED_WR_Byte(0xA4, OLED_CMD);  /* 关闭全部显示（0xA4=正常，0xA5=强制显示）*/
    OLED_WR_Byte(0xA6, OLED_CMD);  /* 设置显示极性（0xA6=正常，0xA7=反转）*/

    OLED_Clear();  /* 清屏 */

    OLED_WR_Byte(0xAF, OLED_CMD);  /* 开启显示 */
}
