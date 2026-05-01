/**
 * @file Key.h
 * @brief 按键驱动头文件
 *
 * 硬件说明：
 *   - 按键类型：无源按键（低电平触发）
 *   - 按键数量：4个（Key0~Key3）
 *   - 读取方式：GPIO引脚读取
 *
 * 按键检测机制：
 *   - 使用定时器中断定时扫描按键状态
 *   - 支持按键按下检测（边沿触发）
 *   - Key_GetNum() 返回1表示按键刚按下，返回0表示无按键事件
 *
 * 按键状态标志位说明：
 *   - KEY_HOLD: 按键长按标志
 *   - KEY_DOWN: 按键按下（边沿）
 *   - KEY_UP: 按键释放（边沿）
 *   - KEY_SINGLE: 单击标志
 *   - KEY_DOUBLE: 双击标志
 *   - KEY_LONG: 长按标志
 *   - KEY_REPEAT: 重复触发标志
 *
 * 时间阈值：
 *   - KEY_TIME_LONG: 长按判定时间（2000ms）
 *   - KEY_TIME_DOUBLE: 双击判定时间（200ms）
 *   - KEY_TIME_REPEAT: 重复触发间隔（100ms）
 */

#ifndef _KEY_H_
#define _KEY_H_

/*================================== 头文件引用 =====================================*/
#include "ti_msp_dl_config.h"  /* TI MSPM0 芯片配置（GPIO引脚定义等）*/

/*================================== 按键状态标志位 =====================================*/
#define KEY_HOLD     0x01   /**< 按键长按标志 */
#define KEY_DOWN     0x02   /**< 按键按下标志（下降沿）*/
#define KEY_UP       0x04   /**< 按键释放标志（上升沿）*/
#define KEY_SINGLE   0x08   /**< 单击标志 */
#define KEY_DOUBLE   0x10   /**< 双击标志 */
#define KEY_LONG     0x20   /**< 长按标志 */
#define KEY_REPEAT   0x40   /**< 重复触发标志 */

/*================================== 按键电平定义 =====================================*/
#define KEY_PRESSED      1   /**< 按键按下状态（低电平）*/
#define KEY_UNPRESSED    0   /**< 按键未按下状态（高电平）*/

/*================================== 时间阈值定义 =====================================*/
#define KEY_TIME_LONG      2000   /**< 长按判定时间（单位：ms）*/
#define KEY_TIME_DOUBLE    200     /**< 双击判定时间（单位：ms）*/
#define KEY_TIME_REPEAT    100     /**< 重复触发间隔（单位：ms）*/

/*================================== 函数声明 =====================================*/

/**
 * @brief 获取Key0按键事件
 * @return uint8_t 返回1表示按键刚按下，返回0表示无事件
 * @note 调用后清除按键事件标志
 */
uint8_t Key0_GetNum(void);

/**
 * @brief 获取Key1按键事件
 * @return uint8_t 返回1表示按键刚按下，返回0表示无事件
 */
uint8_t Key1_GetNum(void);

/**
 * @brief 获取Key2按键事件
 * @return uint8_t 返回1表示按键刚按下，返回0表示无事件
 */
uint8_t Key2_GetNum(void);

/**
 * @brief 获取Key3按键事件
 * @return uint8_t 返回1表示按键刚按下，返回0表示无事件
 */
uint8_t Key3_GetNum(void);

/**
 * @brief Key0定时器扫描函数
 * @note 由定时器中断定期调用（通常10ms一次）
 */
void Key0_Tick(void);

/**
 * @brief Key1定时器扫描函数
 * @note 由定时器中断定期调用
 */
void Key1_Tick(void);

/**
 * @brief Key2定时器扫描函数
 * @note 由定时器中断定期调用
 */
void Key2_Tick(void);

/**
 * @brief Key3定时器扫描函数
 * @note 由定时器中断定期调用
 */
void Key3_Tick(void);

#endif /* _KEY_H_ */
