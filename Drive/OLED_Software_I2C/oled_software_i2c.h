/**
 * @file oled_software_i2c.h
 * @brief OLED显示屏软件I2C驱动头文件
 *
 * 硬件说明：
 *   - 屏幕类型：OLED SSD1306（128x64像素）
 *   - 通信协议：I2C软件模拟
 *   - 控制引脚：SCL（时钟）、SDA（数据）
 *   - I2C地址：通常0x78（7位地址）
 *
 * 显示特点：
 *   - OLED屏幕自发光，无需背光
 *   - 对比度高，视角广，功耗低
 *   - 128x64分辨率，16像素字体可显示8列4行
 *
 * 函数说明：
 *   - OLED_Init(): 初始化OLED屏幕
 *   - OLED_Clear(): 清屏
 *   - OLED_ShowString/Num/Char(): 显示字符/数字
 *   - OLED_Set_Pos(): 设置光标位置
 *
 * 坐标说明：
 *   - x: 列位置（0~127）
 *   - y: 行位置（0~63）
 *   - 屏幕分为8个8像素高的页（0~7）
 */

/*
 * SysConfig Configuration Steps:
 *   GPIO:
 *     1. Add a GPIO module.
 *     2. Name the group as "GPIO_OLED".
 *     3. Name the pins as "PIN_OLED_SCL" and "PIN_OLED_SDA".
 *     4. Set the pins according to your needs.
 */

#ifndef __OLED_SOFTWARE_I2C_H
#define __OLED_SOFTWARE_I2C_H

/*================================== 头文件引用 =====================================*/
#include "ti_msp_dl_config.h"  /* TI MSPM0 芯片配置（GPIO引脚定义等）*/
#include "oledfont.h"         /* OLED字模数据（ASCII字符点阵）*/
#include "clock.h"            /* 系统时钟和延时函数（delay_ms）*/

/*================================== I2C传输类型 =====================================*/
#define OLED_CMD  0   /**< I2C传输类型：命令 */
#define OLED_DATA 1   /**< I2C传输类型：数据 */

/*================================== GPIO端口兼容性 =====================================*/
#ifndef GPIO_OLED_PIN_OLED_SCL_PORT
#define GPIO_OLED_PIN_OLED_SCL_PORT GPIO_OLED_PORT
#endif

#ifndef GPIO_OLED_PIN_OLED_SDA_PORT
#define GPIO_OLED_PIN_OLED_SDA_PORT GPIO_OLED_PORT
#endif

/*================================== 软件I2C引脚控制 =====================================*/
/**
 * @brief OLED I2C时钟线SCL控制
 * @note 软件模拟I2C时序
 */
#define OLED_SCL_Set()    (DL_GPIO_setPins(GPIO_OLED_PIN_OLED_SCL_PORT, GPIO_OLED_PIN_OLED_SCL_PIN))   /**< SCL高电平 */
#define OLED_SCL_Clr()    (DL_GPIO_clearPins(GPIO_OLED_PIN_OLED_SCL_PORT, GPIO_OLED_PIN_OLED_SCL_PIN)) /**< SCL低电平 */

/**
 * @brief OLED I2C数据线SDA控制
 * @note 软件模拟I2C时序
 */
#define OLED_SDA_Set()    (DL_GPIO_setPins(GPIO_OLED_PIN_OLED_SDA_PORT, GPIO_OLED_PIN_OLED_SDA_PIN))   /**< SDA高电平 */
#define OLED_SDA_Clr()    (DL_GPIO_clearPins(GPIO_OLED_PIN_OLED_SDA_PORT, GPIO_OLED_PIN_OLED_SDA_PIN)) /**< SDA低电平 */

/*================================== 函数声明 =====================================*/

/**
 * @brief 延时函数（毫秒级）
 * @param ms 延时时间（毫秒）
 */
void delay_ms(uint32_t ms);

/**
 * @brief OLED颜色反转
 * @param i 0=正常颜色，1=反转颜色
 */
void OLED_ColorTurn(uint8_t i);

/**
 * @brief OLED显示方向设置
 * @param i 0=正常显示，1=180度翻转
 */
void OLED_DisplayTurn(uint8_t i);

/**
 * @brief OLED写一个字节
 * @param dat 要写入的数据
 * @param cmd 传输类型：OLED_CMD=命令，OLED_DATA=数据
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);

/**
 * @brief 设置OLED光标位置
 * @param x 列位置（0~127）
 * @param y 行位置（0~63）
 */
void OLED_Set_Pos(uint8_t x, uint8_t y);

/**
 * @brief OLED打开显示
 */
void OLED_Display_On(void);

/**
 * @brief OLED关闭显示
 */
void OLED_Display_Off(void);

/**
 * @brief OLED清屏
 * @note 将所有像素关闭，显示全黑
 */
void OLED_Clear(void);

/**
 * @brief OLED显示一个字符
 * @param x 列位置（0~127）
 * @param y 行位置（0~63）
 * @param chr 要显示的字符
 * @param sizey 字体大小（通常16像素高度）
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t sizey);

/**
 * @brief 整数次方计算
 * @param m 底数
 * @param n 指数
 * @return uint32_t 计算结果
 */
uint32_t oled_pow(uint8_t m, uint8_t n);

/**
 * @brief OLED显示数字
 * @param x 列位置（0~127）
 * @param y 行位置（0~63）
 * @param num 要显示的数字（无符号32位）
 * @param len 数字位数
 * @param sizey 字体大小（通常16像素高度）
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t sizey);

/**
 * @brief OLED显示字符串
 * @param x 列位置（0~127）
 * @param y 行位置（0~63）
 * @param chr 字符串指针
 * @param sizey 字体大小（通常16像素高度）
 */
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t sizey);

/**
 * @brief OLED显示汉字
 * @param x 列位置（0~127）
 * @param y 行位置（0~63）
 * @param no 汉字在数组中的索引
 * @param sizey 字体大小（通常16像素高度）
 */
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t no, uint8_t sizey);

/**
 * @brief OLED显示图片
 * @param x 起始列位置
 * @param y 起始行位置
 * @param sizex 图片宽度（像素）
 * @param sizey 图片高度（像素）
 * @param BMP 图片数据数组
 */
void OLED_DrawBMP(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, uint8_t BMP[]);

/**
 * @brief OLED初始化
 * @note 初始化I2C引脚、发送初始化序列、打开显示
 */
void OLED_Init(void);

#endif /* __OLED_SOFTWARE_I2C_H */
