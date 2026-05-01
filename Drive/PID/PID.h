/**
 * @file PID.h
 * @brief PID控制器头文件
 *
 * 本文件实现了两种PID控制器：
 *
 * 1. 增量式PID控制器 (PID_Controller)
 *    - 用于电机速度控制
 *    - 特点：计算增量Δu(k)，输出为累积值
 *    - 公式：Δu(k) = Kp[e(k)-e(k-1)] + Ki·dt·e(k) + Kd[e(k)-2e(k-1)+e(k-2)]/dt
 *
 * 2. 位置式PID控制器 (PID_Positional_Controller)
 *    - 用于灰度传感器循线控制
 *    - 特点：直接计算绝对输出量u(k)
 *    - 公式：u(k) = Kp·e(k) + Ki·Σe(k) + Kd·e(k)/dt
 *
 * 两种PID的区别：
 *   - 增量式：输出是控制量的变化量，适合积分器本身就是执行机构的场景
 *   - 位置式：输出是绝对控制量，适合执行机构直接跟随的场景
 */

#ifndef _PID_H
#define _PID_H

/*================================== 头文件引用 =====================================*/
#include <math.h>           /* 数学函数（fabsf等）*/
#include "BlueSerial.h"     /* 蓝牙串口（调试输出）*/

/*================================== 预设PID参数 =====================================*/
/**
 * @brief 电机PID预设参数
 * @note 这些参数可根据实际电机和编码器特性调整
 *       KP：比例系数，影响响应速度
 *       KI：积分系数，消除稳态误差
 *       KD：微分系数，抑制超调
 */
#define PID_PRESET_KP (0.55f)    /**< 比例系数 */
#define PID_PRESET_KI (0.75f)    /**< 积分系数 */
#define PID_PRESET_KD (0.00f)  /**< 微分系数 */
// #define PID_PRESET_KD (0.0005f)  /**< 微分系数 */

/*================================== 增量式PID控制器结构体 =====================================*/
/**
 * @brief 增量式PID控制器结构体
 *
 * 用于电机速度控制，计算控制增量
 *
 * 主要参数：
 *   - kp, ki, kd: PID系数
 *   - dt: 控制周期（秒）
 *   - filter_alpha: 一阶滤波系数 [0,1]
 *   - dead_zone: 误差死区
 *   - max_speed: 速度上限
 *   - integral_limit: 积分限幅
 *
 * 状态变量：
 *   - err: 当前原始误差
 *   - prev_filtered_err: 上一次滤波误差 e(k-1)
 *   - prev_prev_filtered_err: 上上次滤波误差 e(k-2)
 *   - derivative: 微分项（存储Δu(k)）
 *   - output: 最终输出
 *   - first: 首次运行标志（用于初始化滤波）
 */
typedef struct
{
    /*------ PID系数 ------*/
    float kp;              /**< 比例系数 */
    float ki;              /**< 积分系数 */
    float kd;              /**< 微分系数 */

    /*------ 控制参数 ------*/
    float dt;              /**< 控制周期（秒），如1ms=0.001s */
    float filter_alpha;    /**< 一阶滤波系数，范围[0,1]，越大越跟随当前误差 */
    float dead_zone;       /**< 误差死区阈值，绝对值小于此值时视为0 */
    float max_speed;       /**< 最大速度限幅（同时限制目标和输出）*/
    float integral_limit;  /**< 积分项限幅，防止积分饱和 */

    /*------ 状态变量 ------*/
    float err;                     /**< 当前原始误差（死区处理后）*/
    float prev_filtered_err;       /**< 上一次滤波误差 e(k-1) */
    float prev_prev_filtered_err;  /**< 上上次滤波误差 e(k-2)，新增字段 */
    float integral;                /**< 积分累积值（保留，兼容结构体）*/
    float derivative;               /**< 微分项（存储Δu(k)）*/
    float output;                  /**< 最终控制输出（已限幅）*/

    int first;                     /**< 首次运行标志：1=首次，0=非首次 */
} PID_Controller;

/*================================== PWM输出命令结构体 =====================================*/
/**
 * @brief PWM输出命令结构体
 *
 * 用于封装PID输出转换为电机PWM所需的参数
 */
typedef struct
{
    int direction;   /**< 方向：1=正转，-1=反转，0=停止 */
    float duty;     /**< PWM占空比：范围 -1.0~1.0（负数表示反转）*/
} PID_PWMCommand;

/*================================== 位置式PID控制器结构体 =====================================*/
/**
 * @brief 位置式PID控制器结构体
 *
 * 用于灰度传感器循线控制，直接计算绝对输出量
 *
 * 主要参数：
 *   - kp, ki, kd: PID系数
 *   - dt: 控制周期（秒）
 *   - integral_limit: 积分限幅
 *   - derivative_filter: 微分一阶滤波系数
 *   - dead_zone: 误差死区
 *   - output_limit: 输出限幅
 *
 * 状态变量：
 *   - err: 当前误差 e(k)
 *   - prev_err: 上一次误差 e(k-1)
 *   - prev_derivative: 上一次微分项（用于滤波）
 *   - integral: 积分累积值
 *   - output: 最终控制输出
 */
typedef struct
{
    /*------ PID系数 ------*/
    float kp;              /**< 比例系数 */
    float ki;              /**< 积分系数 */
    float kd;              /**< 微分系数 */

    /*------ 控制参数 ------*/
    float dt;                   /**< 控制周期（秒）*/
    float integral_limit;       /**< 积分项限幅，防止积分饱和 */
    float derivative_filter;     /**< 微分一阶滤波系数，范围[0,1] */
    float dead_zone;            /**< 误差死区阈值 */
    float output_limit;         /**< 输出限幅 */

    /*------ 状态变量 ------*/
    float err;              /**< 当前误差 e(k) */
    float prev_err;         /**< 上一次误差 e(k-1) */
    float prev_derivative;  /**< 上一次微分项（滤波用）*/
    float integral;         /**< 积分累积值 */
    float output;           /**< 最终控制输出 */
} PID_Positional_Controller;

/*================================== 位置式PID函数声明 =====================================*/

/**
 * @brief 位置式PID初始化
 *
 * @param pid PID控制器结构体指针
 * @param kp 比例系数
 * @param ki 积分系数
 * @param kd 微分系数
 * @param dt 控制周期（秒）
 * @param integral_limit 积分限幅
 * @param derivative_filter 微分一阶滤波系数 [0,1]
 * @param dead_zone 误差死区阈值
 * @param output_limit 输出限幅
 */
void PID_Positional_Init(PID_Positional_Controller *pid,
                         float kp,
                         float ki,
                         float kd,
                         float dt,
                         float integral_limit,
                         float derivative_filter,
                         float dead_zone,
                         float output_limit);

/**
 * @brief 位置式PID重置
 *
 * @param pid PID控制器结构体指针
 *
 * @note 将所有状态变量清零
 */
void PID_Positional_Reset(PID_Positional_Controller *pid);

/**
 * @brief 位置式PID更新计算
 *
 * @param pid PID控制器结构体指针
 * @param target 目标值
 * @param measured 测量值（当前值）
 * @return PID控制输出
 *
 * 位置式PID公式：
 *   u(k) = Kp·e(k) + Ki·Σe(k) + Kd·(e(k)-e(k-1))/dt
 *
 * @note 用于灰度传感器循线转向控制
 */
float PID_Positional_Update(PID_Positional_Controller *pid, float target, float measured);

/*================================== 增量式PID函数声明 =====================================*/

/**
 * @brief 增量式PID初始化（完整参数版）
 *
 * @param pid PID控制器结构体指针
 * @param kp 比例系数
 * @param ki 积分系数
 * @param kd 微分系数
 * @param dt 控制周期（秒）
 * @param filter_alpha 一阶滤波系数 [0,1]
 * @param dead_zone 误差死区阈值
 * @param max_speed 最大速度限幅
 * @param integral_limit 积分限幅
 */
void PID_Init(PID_Controller *pid,
              float kp,
              float ki,
              float kd,
              float dt,
              float filter_alpha,
              float dead_zone,
              float max_speed,
              float integral_limit);

/**
 * @brief 增量式PID初始化（使用预设参数版）
 *
 * @param pid PID控制器结构体指针
 * @param dt 控制周期（秒）
 * @param filter_alpha 一阶滤波系数 [0,1]
 * @param dead_zone 误差死区阈值
 * @param max_speed 最大速度限幅
 * @param integral_limit 积分限幅
 *
 * @note PID系数使用预设值 PID_PRESET_KP/KI/KD
 */
void PID_InitPreset(PID_Controller *pid,
                    float dt,
                    float filter_alpha,
                    float dead_zone,
                    float max_speed,
                    float integral_limit);

/**
 * @brief 增量式PID重置
 *
 * @param pid PID控制器结构体指针
 *
 * @note 将所有状态变量清零，首次标志置1
 */
void PID_Reset(PID_Controller *pid);

/**
 * @brief 增量式PID更新计算
 *
 * @param pid PID控制器结构体指针
 * @param target_speed 目标速度
 * @param measured_speed 测量速度（实际速度）
 * @return PID控制输出（速度值）
 *
 * 增量式PID公式：
 *   Δu(k) = Kp[e(k)-e(k-1)] + Ki·dt·e(k) + Kd[e(k)-2e(k-1)+e(k-2)]/dt
 *   u(k) = u(k-1) + Δu(k)
 *
 * @note 用于电机速度控制
 */
float PID_Update(PID_Controller *pid, float target_speed, float measured_speed);

/**
 * @brief PID输出转换为PWM命令
 *
 * @param pid PID控制器指针
 * @param pid_output PID输出值
 * @param pwm_min_duty PWM最小占空比
 * @param pwm_max_duty PWM最大占空比
 * @param cmd PWM命令输出结构体指针
 *
 * @note 根据PID输出方向和绝对值计算PWM占空比
 */
void PID_OutputToPWM(const PID_Controller *pid,
                     float pid_output,
                     float pwm_min_duty,
                     float pwm_max_duty,
                     PID_PWMCommand *cmd);

/**
 * @brief PID更新并转换为PWM命令（合并函数）
 *
 * @param pid PID控制器结构体指针
 * @param target_speed 目标速度
 * @param measured_speed 测量速度
 * @param pwm_min_duty PWM最小占空比
 * @param pwm_max_duty PWM最大占空比
 * @param cmd PWM命令输出结构体指针
 *
 * @note 先调用PID_Update计算输出，再调用PID_OutputToPWM转换
 */
void PID_UpdateToPWM(PID_Controller *pid,
                     float target_speed,
                     float measured_speed,
                     float pwm_min_duty,
                     float pwm_max_duty,
                     PID_PWMCommand *cmd);

#endif /* _PID_H */
