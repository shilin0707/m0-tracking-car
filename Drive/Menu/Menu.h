/**
 * @file Menu.h
 * @brief 菜单系统头文件
 *
 * 菜单系统采用两级结构：
 *   - 第一级：Menu_StateMachine() -> Menu_First() 主菜单，用于选择子菜单
 *   - 第二级：Menu_Second_X() 子菜单，各自独立的状态机
 *
 * 子菜单功能说明：
 *   - Menu_Second_1: 灰度循线模式（电机PID + 灰度传感器PID）
 *   - Menu_Second_2: 蓝牙摇杆控制模式（电机PID + 蓝牙串口控制）
 *   - Menu_Second_3: 计数器演示（无电机控制）
 *   - Menu_Second_4: 计数器演示（无电机控制）
 */

#ifndef _MENU_H_
#define _MENU_H_

/*================================== 头文件引用 =====================================*/
#include "oled_software_i2c.h"    /* OLED显示屏驱动（菜单显示）*/
#include "Key.h"                  /* 按键输入驱动（菜单操作）*/
#include "Buzzer.h"               /* 蜂鸣器驱动（操作提示音）*/
#include "pwm.h"                  /* PWM生成驱动（电机调速）*/
#include "motor.h"                /* 电机控制驱动（速度计算）*/
#include "Encoder.h"              /* 编码器读取驱动（速度反馈）*/
#include "PID.h"                  /* PID控制器驱动（速度/转向控制）*/
#include "BlueSerial.h"           /* 蓝牙串口通信驱动（遥控数据）*/
#include "wit.h"                  /* 陀螺仪驱动（姿态数据）*/
#include "grayscale_sensor.h"     /* 八路灰度传感器驱动（循线检测）*/

/*================================== 主菜单状态宏 =====================================*/
/**
 * @brief 主菜单状态宏
 * @note 用于 Menu_StateMachine() 的 switch 语句，选择进入哪个子菜单
 */
#define STATE_MENU_1     1   /**< 子菜单1：灰度循线模式 */
#define STATE_MENU_2     2   /**< 子菜单2：蓝牙摇杆控制模式 */
#define STATE_MENU_3     3   /**< 子菜单3：计数器演示 */
#define STATE_MENU_4     4   /**< 子菜单4：计数器演示 */

/*================================== 子菜单内部状态宏 =====================================*/
/**
 * @brief 子菜单通用状态机状态
 * @note 各 Menu_Second_X() 函数内部使用的状态定义
 *
 * 状态转换图：
 *   [停止态] --K1--> [运行态]
 *   [运行态] --K2--> [停止态]
 *   [停止态] --K3--> [退出]
 */
#define MENU_STATE_STOP    0   /**< 停止态：等待用户操作 */
#define MENU_STATE_RUNNING 1   /**< 运行态：执行任务 */

/*================================== 全局运行标志 =====================================*/
/**
 * @brief 全局电机运行标志
 * @details 供电机定时器和编码器中断判断是否执行电机控制
 * - Menu1 和 Menu2 启动时设为 1，停止时设为 0
 * - Menu3 和 Menu4 不涉及电机，无需设置
 *
 * @note 注意：此变量在 Menu.c 中定义
 */
extern uint8_t g_motor_running;

/*================================== 蓝牙控制模式变量 =====================================*/
/**
 * @brief 蓝牙控制模式标志
 * @details 用于切换电机PID的目标速度来源
 *
 * 控制模式：
 * - g_blueserial_control_enable = 1: 使用 g_target_speed_L/R（Menu2 蓝牙摇杆控制）
 * - g_blueserial_control_enable = 0: 使用 BASE_SPEED +/- 灰度PID输出（Menu1 循线）
 *
 * @note 注意：此变量在 Menu.c 中定义
 */
extern uint8_t g_blueserial_control_enable;

/**
 * @brief 左轮目标速度
 * @details 由 BlueSerial_Control() 根据摇杆值计算得出
 *         电机定时器中断中使用此值作为PID的目标速度
 *
 * @note 注意：此变量在 Menu.c 中定义
 */
extern float g_target_speed_L;

/**
 * @brief 右轮目标速度
 * @details 由 BlueSerial_Control() 根据摇杆值计算得出
 *         电机定时器中断中使用此值作为PID的目标速度
 *
 * @note 注意：此变量在 Menu.c 中定义
 */
extern float g_target_speed_R;

/*================================== 菜单内部变量 =====================================*/
/**
 * @brief 主菜单当前选中的菜单编号
 * @note 由 Menu_First() 更新，Menu_StateMachine() 读取
 */
extern uint8_t state;

/**
 * @brief 电机定时器中断分频计数器
 * @note 每10次中断执行一次完整的电机PID计算和PWM输出
 */
extern uint8_t Count;

/*================================== PID控制器结构体 =====================================*/
/**
 * @brief 左右电机增量式PID控制器
 * @note 由 Menu1（灰度循线）启动时初始化
 */
extern PID_Controller PID_Motor_L;
extern PID_Controller PID_Motor_R;

/**
 * @brief 左右电机PWM命令结构体
 * @note 存储PID输出转换后的PWM占空比和方向
 */
extern PID_PWMCommand PWMCommand_L;
extern PID_PWMCommand PWMCommand_R;

/**
 * @brief 灰度传感器位置式PID控制器
 * @note 由 Menu1（灰度循线）启动时初始化，用于计算转向控制量
 */
extern PID_Positional_Controller PID_Grayscale_Sensor;

/*================================== 电机PID参数宏 =====================================*/
/**
 * @brief 电机PID控制器参数
 *
 * MAX_SPEED: 编码器测量最大速度（用于PID输出限幅）
 * FILTERRED_ALGHA: 一阶滤波系数，范围 [0,1]，越大越跟随当前误差
 * DEAD_ZONE: 误差死区，低于此值时视为0
 * INTEGRAL_LIMIT: 积分项限幅，防止积分饱和
 * DT: 控制周期（秒），1ms = 0.001s
 * PWM_MAX_DUTY: PWM最大占空比
 * PWM_MIN_DUTY: PWM最小占空比（启动时需要克服静摩擦）
 * BASE_SPEED: 循线模式下的基础速度
 */
#define MAX_SPEED        1021.017595f  /**< 电机最大速度（编码器测量值） */
#define FILTERRED_ALGHA  0.15f         /**< 一阶滤波系数 */
#define DEAD_ZONE       1.0f          /**< 误差死区阈值 */
#define INTEGRAL_LIMIT  1000.0f       /**< 积分项限幅值 */
#define DT              0.001f         /**< PID控制周期（秒）*/
#define PWM_MAX_DUTY    1.0f          /**< PWM最大占空比 */
#define PWM_MIN_DUTY    0.05f         /**< PWM最小占空比（启动最小速度） */
#define BASE_SPEED      250.0f         /**< 循线模式基础速度（降低以增大速度差调节空间）*/

/*================================== 灰度传感器PID参数宏 =====================================*/
/**
 * @brief 八路灰度传感器循线PID参数
 *
 * 使用位置式PID，根据灰度传感器检测的黑线位置偏差计算转向控制量
 * 目标值 4.5 表示小车居中于黑线
 *
 * @note 这些参数需要根据实际赛道情况调整
 */
  #define Grayscale_Sensor_KP             29.48f
  #define Grayscale_Sensor_KI             0.1417f
  #define Grayscale_Sensor_KD             4.9140f
// #define Grayscale_Sensor_KP             30.0f   // 增大比例系数，增强响应
// #define Grayscale_Sensor_KI             0.0f   // 增大积分系数
// #define Grayscale_Sensor_KD             0.0f   // 增大微分系数，增强预测
// #define Grayscale_Sensor_integral_limit     50.0f   /**< 积分限幅 */
// #define Grayscale_Sensor_derivative_filter  0.15f   /**< 微分一阶滤波系数 */
// #define Grayscale_Sensor_dead_zone          1.0f   /**< 误差死区 */
// #define Grayscale_Sensor_output_limit       100.0f   /**< 输出限幅（最大速度差）*/
/* 控制参数 (调整后) */
  #define Grayscale_Sensor_dt             0.010f /**< PID控制周期（秒）*/
  #define Grayscale_Sensor_integral_limit     80.0f   // 增大积分限幅
  #define Grayscale_Sensor_derivative_filter  0.30f   // 减小滤波，加快响应
  #define Grayscale_Sensor_dead_zone          0.50f   // 减小死区，增强转向响应
  #define Grayscale_Sensor_output_limit       300.0f  // 增大输出限幅到180，增强最大转向力度
/*================================== 函数声明 =====================================*/

/**
 * @brief 主菜单状态机入口
 * @details 循环调用 Menu_First() 获取用户选择，然后进入对应的子菜单
 */
void Menu_StateMachine(void);

/**
 * @brief 主菜单选择界面
 * @details 显示4个菜单选项，通过K1/K2上下选择，K3确认
 * @return uint8_t 返回用户选择的菜单编号 (STATE_MENU_1 ~ STATE_MENU_4)
 */
int Menu_First(void);

/**
 * @brief 子菜单1：灰度循线模式
 * @details 使用两状态非阻塞状态机：
 *          - 停止态：等待K1启动，K3退出
 *          - 运行态：执行灰度PID循线，K2停止
 *
 * 状态转换：
 *   [停止态] --K1--> [运行态]
 *   [运行态] --K2--> [停止态]
 *   [停止态] --K3--> [退出]
 *
 * @return int 退出时返回0
 */
int Menu_Second_1(void);

/**
 * @brief 子菜单2：蓝牙摇杆控制模式
 * @details 使用两状态非阻塞状态机：
 *          - 停止态：等待K1启动，K3退出
 *          - 运行态：执行蓝牙PID控制，K2停止
 *
 * 蓝牙控制：
 *          - 接收蓝牙串口的摇杆数据
 *          - LV控制基础速度，RV控制转向差
 *          - 目标速度传递给电机PID控制器
 *
 * 状态转换：
 *   [停止态] --K1--> [运行态]
 *   [运行态] --K2--> [停止态]
 *   [停止态] --K3--> [退出]
 *
 * @return int 退出时返回0
 */
int Menu_Second_2(void);

/**
 * @brief 子菜单3：计数器演示
 * @details 使用两状态非阻塞状态机：
 *          - 停止态：等待K1启动，K3退出
 *          - 运行态：计数器递增显示，K2停止
 *
 * 状态转换：
 *   [停止态] --K1--> [运行态]
 *   [运行态] --K2--> [停止态]
 *   [停止态] --K3--> [退出]
 *
 * @return int 退出时返回0
 */
int Menu_Second_3(void);

/**
 * @brief 子菜单4：计数器演示
 * @details 使用两状态非阻塞状态机：
 *          - 停止态：等待K1启动，K3退出
 *          - 运行态：计数器递增显示，K2停止
 *
 * 状态转换：
 *   [停止态] --K1--> [运行态]
 *   [运行态] --K2--> [停止态]
 *   [停止态] --K3--> [退出]
 *
 * @return int 退出时返回0
 */
int Menu_Second_4(void);

#endif /* _MENU_H_ */
