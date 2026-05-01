/**
 * @file Menu.c
 * @brief 菜单系统源文件
 *
 * 菜单系统采用两级结构设计：
 *   - 第一级：Menu_StateMachine() -> Menu_First() 主菜单，用于选择子菜单
 *   - 第二级：Menu_Second_X() 子菜单，各自独立的两状态非阻塞状态机
 *
 * 状态机设计模式：
 *   - 所有子菜单使用统一的双状态机结构 (MENU_STATE_STOP / MENU_STATE_RUNNING)
 *   - 按键采用锁定机制防止重复触发
 *   - 非阻塞设计，主循环持续检测按键和执行任务
 *
 * 按键约定：
 *   - K1: 启动键（停止态有效）
 *   - K2: 停止键（运行态有效）
 *   - K3: 退出键（停止态有效）
 *
 * 全局控制标志：
 *   - g_motor_running: 电机运行总开关，控制编码器中断和PID中断是否执行
 *   - g_blueserial_control_enable: 控制模式切换（灰度循线 vs 蓝牙摇杆）
 *   - g_target_speed_L/R: 蓝牙控制模式下的目标速度
 */

#include "Menu.h"

/*================================== 全局变量 =====================================*/
uint8_t state;          /**< 主菜单当前选中的菜单编号 (1~4) */

/** @brief 全局电机运行标志，详见 Menu.h */
uint8_t g_motor_running = 0;

/** @brief 蓝牙控制模式标志，详见 Menu.h */
uint8_t g_blueserial_control_enable = 0;  /* BlueSerial控制模式标志 */
/** @brief 左轮目标速度，详见 Menu.h */
float g_target_speed_L = 0;
/** @brief 右轮目标速度，详见 Menu.h */
float g_target_speed_R = 0;

/*================================== PID控制器结构体 =====================================*/
PID_Controller PID_Motor_L, PID_Motor_R;            /**< 左右电机增量式PID控制器 */
PID_PWMCommand PWMCommand_L, PWMCommand_R;          /**< 左右电机PWM命令结构体 */
PID_Positional_Controller PID_Grayscale_Sensor;    /**< 灰度传感器位置式PID控制器 */


/**
 * @brief 主菜单状态机入口
 *
 * 程序的主入口函数，循环执行以下流程：
 *   1. 调用 Menu_First() 显示主菜单，等待用户选择
 *   2. 根据用户选择进入对应的子菜单 (Menu_Second_1~4)
 *   3. 子菜单退出后，返回此处继续显示主菜单
 *
 * @note 这是一个无限循环函数，不会返回
 *
 * 执行流程图：
 *   [Menu_StateMachine] --> [Menu_First选择] --> [Menu_Second_X]
 *           ^                                              |
 *           |____________________ 从子菜单返回 _____________|
 */
void Menu_StateMachine(void)
{
    while (1)
    {
        /* 显示主菜单，获取用户选择的菜单编号 */
        state = Menu_First();

        /* 根据选择进入对应的子菜单 */
        switch(state)
        {
        case STATE_MENU_1:
            Menu_Second_1();  /* 灰度循线模式 */
            break;
        case STATE_MENU_2:
            Menu_Second_2();  /* 蓝牙摇杆控制模式 */
            break;
        case STATE_MENU_3:
            Menu_Second_3();  /* 计数器演示 */
            break;
        case STATE_MENU_4:
            Menu_Second_4();  /* 计数器演示 */
            break;
        }
        /* 子菜单返回后继续循环，显示主菜单 */
    }
}

/**
 * @brief 主菜单选择界面
 *
 * 显示4个菜单选项供用户选择，通过K1/K2上下移动光标，K3确认选择
 *
 * 按键操作：
 *   - K1: 上移光标（flag递减）
 *   - K2: 下移光标（flag递增）
 *   - K3: 确认选择，返回当前选中的菜单编号
 *
 * 界面布局（OLED 128x64，16像素字体）：
 *   - 星号(*)表示当前选中项
 *   - 第0行：model_1 (或*)
 *   - 第2行：model_2 (或*)
 *   - 第4行：model_3 (或*)
 *   - 第6行：model_4 (或*)
 *
 * @return int 返回用户选择的菜单编号 (STATE_MENU_1 ~ STATE_MENU_4)
 *
 * @note 此函数阻塞等待用户确认选择
 */
int Menu_First(void)
{
    uint8_t flag = 1;  /* 当前选中的菜单项编号 (1~4) */

    /* 初始化显示：先清屏，再显示所有菜单项 */
    OLED_ShowString(16, 0, (uint8_t *)"*", 16);       /* 光标星号 */
    OLED_ShowString(24, 0, (uint8_t *)"model_1", 16);  /* 第1项 */
    OLED_ShowString(24, 2, (uint8_t *)"model_2", 16);  /* 第2项 */
    OLED_ShowString(24, 4, (uint8_t *)"model_3", 16);  /* 第3项 */
    OLED_ShowString(24, 6, (uint8_t *)"model_4", 16);  /* 第4项 */

    /* 主循环：持续检测按键，更新显示 */
    while (1)
    {
        /* K1按下：上移光标（flag递减）*/
        if (Key1_GetNum())
        {
            DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);  /* LED翻转提示 */
            Buzzer_Delay_ON();  /* 蜂鸣器提示音 */
            flag--;  /* 向上移动选择 */
        }

        /* K2按下：下移光标（flag递增）*/
        if (Key2_GetNum())
        {
            DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);  /* LED翻转提示 */
            Buzzer_Delay_ON();  /* 蜂鸣器提示音 */
            flag++;  /* 向下移动选择 */
        }

        /* K3按下：确认选择，返回当前菜单编号 */
        if (Key3_GetNum())
        {
            DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);  /* LED翻转提示 */
            Buzzer_Delay_ON();  /* 蜂鸣器提示音 */
            return flag;  /* 返回选中的菜单编号 */
        }

        /* 边界处理：循环滚动 */
        if (flag > 4)  /* 超过最后一项时回到第一项 */
        {
            flag = 1;
        }
        if (flag < 1)  /* 超过第一项时回到最后一项 */
        {
            flag = 4;
        }

        /* 更新OLED显示：根据flag值确定星号位置 */
        switch (flag)
        {
            case 1:
                /*-------- 选中第1项 --------*/
                OLED_ShowString(16, 0, (uint8_t *)"*", 16);   /* 第1行显示星号 */
                OLED_ShowString(16, 2, (uint8_t *)" ", 16);   /* 清除第2行星号 */
                OLED_ShowString(16, 4, (uint8_t *)" ", 16);   /* 清除第3行星号 */
                OLED_ShowString(16, 6, (uint8_t *)" ", 16);   /* 清除第4行星号 */
                break;

            case 2:
                /*-------- 选中第2项 --------*/
                OLED_ShowString(16, 2, (uint8_t *)"*", 16);   /* 第2行显示星号 */
                OLED_ShowString(16, 0, (uint8_t *)" ", 16);   /* 清除第1行星号 */
                OLED_ShowString(16, 4, (uint8_t *)" ", 16);   /* 清除第3行星号 */
                OLED_ShowString(16, 6, (uint8_t *)" ", 16);   /* 清除第4行星号 */
                break;

            case 3:
                /*-------- 选中第3项 --------*/
                OLED_ShowString(16, 4, (uint8_t *)"*", 16);   /* 第3行显示星号 */
                OLED_ShowString(16, 0, (uint8_t *)" ", 16);   /* 清除第1行星号 */
                OLED_ShowString(16, 2, (uint8_t *)" ", 16);   /* 清除第2行星号 */
                OLED_ShowString(16, 6, (uint8_t *)" ", 16);   /* 清除第4行星号 */
                break;

            case 4:
                /*-------- 选中第4项 --------*/
                OLED_ShowString(16, 6, (uint8_t *)"*", 16);   /* 第4行显示星号 */
                OLED_ShowString(16, 0, (uint8_t *)" ", 16);   /* 清除第1行星号 */
                OLED_ShowString(16, 2, (uint8_t *)" ", 16);   /* 清除第2行星号 */
                OLED_ShowString(16, 4, (uint8_t *)" ", 16);   /* 清除第3行星号 */
                break;
        }
        /* 重新显示所有菜单文本（清除旧字符残留）*/
        OLED_ShowString(24, 0, (uint8_t *)"model_1", 16);
        OLED_ShowString(24, 2, (uint8_t *)"model_2", 16);
        OLED_ShowString(24, 4, (uint8_t *)"model_3", 16);
        OLED_ShowString(24, 6, (uint8_t *)"model_4", 16);
    }
    /* 此处不会执行，函数在K3按下时直接返回 */
    return 0;
}

/**
 * @brief Menu_Second_1 菜单函数
 *
 * 使用两状态非阻塞状态机实现启动/停止/退出控制
 * 包含完整的电机PID控制和灰度传感器循线功能
 *
 * 状态转换图:
 *   [停止态] --K1--> [运行态]
 *   [运行态] --K2--> [停止态]
 *   [停止态] --K3--> [退出]
 *
 * 按键说明:
 *   K1: 启动(停止态有效)
 *   K2: 停止(运行态有效)
 *   K3: 退出(仅在停止态有效)
 *
 * @return int 退出时返回0
 */
int Menu_Second_1(void)
{
    /*------------------------------ 变量声明 ------------------------------*/
    uint8_t state = MENU_STATE_STOP;     /* 状态机当前状态: 0=停止态 */

    /* 按键锁定标志位
     * 作用: 防止按键按住不放时同一动作被重复触发
     * 原理: 按下时锁定=1，此时忽略后续按下；松开后锁定清0，允许下次触发
     */
    uint8_t key1_lock = 0;  /* K1锁定标志 */
    uint8_t key2_lock = 0;  /* K2锁定标志 */
    uint8_t key3_lock = 0;  /* K3锁定标志 */

    /*------------------------------ 初始化显示 ------------------------------*/
    OLED_Clear();                                          /* 清屏 */
    OLED_ShowString(24, 0, (uint8_t *)"model_1", 16);  /* 显示标题 */
    OLED_ShowString(24, 2, (uint8_t *)"stop", 16);        /* 显示初始状态 */

    /*------------------------------ 主循环 ------------------------------*/
    while (1)
    {
        /*----------------------- 按键读取 -----------------------*/
        /* Key_GetNum() 返回1表示按键按下，返回0表示松开 */
        uint8_t key1 = Key1_GetNum();
        uint8_t key2 = Key2_GetNum();
        uint8_t key3 = Key3_GetNum();

        /*----------------------- 状态机处理 -----------------------*/
        switch (state)
        {
            /*========================= 停止态 =========================*/
            case MENU_STATE_STOP:
                /* 此状态下电机停止，等待用户操作 */

                /* K1启动: 按下K1且未锁定时进入运行态 */
                if (key1 && !key1_lock)
                {
                    key1_lock = 1;  /* 锁定K1，防止重复触发 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN); /* LED翻转提示 */
                    Buzzer_Delay_ON();  /* 蜂鸣器提示音 */
                    state = MENU_STATE_RUNNING;  /* 切换到运行态 */
                    OLED_ShowString(24, 2, (uint8_t *)"start", 16);  /* 更新显示状态 */

                    /*---------- 启动初始化代码(仅执行一次) ----------*/
                    // 开启编码器GPIO外部中断
                    NVIC_EnableIRQ(GPIO_Encoder_INT_IRQN);

                    // 电机PID初始化
                    PID_InitPreset(&PID_Motor_L, DT, FILTERRED_ALGHA, DEAD_ZONE, MAX_SPEED, INTEGRAL_LIMIT);
                    PID_InitPreset(&PID_Motor_R, DT, FILTERRED_ALGHA, DEAD_ZONE, MAX_SPEED, INTEGRAL_LIMIT);

                    // 电机控制结构体初始化
                    PID_Motor_L.first = 1;
                    PID_Motor_R.first = 1;

                    // 八路灰度PID初始化并重置（确保历史状态清零）
                    PID_Positional_Init(&PID_Grayscale_Sensor,
                                        Grayscale_Sensor_KP,
                                        Grayscale_Sensor_KI,
                                        Grayscale_Sensor_KD,
                                        Grayscale_Sensor_dt,
                                        Grayscale_Sensor_integral_limit,
                                        Grayscale_Sensor_derivative_filter,
                                        Grayscale_Sensor_dead_zone,
                                        Grayscale_Sensor_output_limit);
                    // 重置灰度PID所有状态（err, prev_err, prev_derivative, integral, output）
                    PID_Grayscale_Sensor.err = 0.0f;
                    PID_Grayscale_Sensor.prev_err = 0.0f;
                    PID_Grayscale_Sensor.prev_derivative = 0.0f;
                    PID_Grayscale_Sensor.integral = 0.0f;
                    PID_Grayscale_Sensor.output = 0.0f;

                    // 输出PWM的中断开始计时
                    DL_TimerG_startCounter(PWM_MOTOR_INST);

                    // 初始PWM输出（最小占空比）
                    Motor_SetPWM(1, PWM_MIN_DUTY);
                    Motor_SetPWM(2, PWM_MIN_DUTY);

                    // 开启速度测量定时器（先启动测速中断）
                    NVIC_EnableIRQ(TIMER_Speed_INST_INT_IRQN);
                    DL_TimerG_startCounter(TIMER_Speed_INST);

                    // 开启外设中断 + 启动定时器（后启动电机中断）
                    NVIC_EnableIRQ(TIMER_Motor_INST_INT_IRQN);
                    DL_TimerG_startCounter(TIMER_Motor_INST);

                    // 八路灰度定时器初始化（后启动灰度传感器中断）
                    NVIC_EnableIRQ(TIMER_Grayscale_Sensor_INST_INT_IRQN);
                    DL_TimerG_startCounter(TIMER_Grayscale_Sensor_INST);

                    // 最后设置全局运行标志，启动PID控制
                    g_motor_running = 1;  /* 设置全局运行标志，供中断使用 */
                }
                /* 按键松开后解除锁定，允许下次触发 */
                if (!key1) key1_lock = 0;

                /* K3退出: 按下K3且未锁定时退出菜单 */
                if (key3 && !key3_lock)
                {
                    key3_lock = 1;  /* 锁定K3 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);
                    Buzzer_Delay_ON();
                    OLED_Clear();  /* 退出前清屏 */
                    return 0;  /* 退出函数 */
                }
                if (!key3) key3_lock = 0;
                break;

            /*========================= 运行态 =========================*/
            case MENU_STATE_RUNNING:
                /* 此状态下电机运行，执行PID控制和循线 */

                /* K2停止: 按下K2且未锁定时返回停止态 */
                if (key2 && !key2_lock)
                {
                    key2_lock = 1;  /* 锁定K2 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);
                    Buzzer_Delay_ON();
                    state = MENU_STATE_STOP;  /* 切换到停止态 */
                    OLED_ShowString(24, 2, (uint8_t *)"stop", 16);  /* 更新显示状态 */

                    /*---------- 停止代码(仅执行一次) ----------*/
                    Motor_SetPWM(1, 0);
                    Motor_SetPWM(2, 0);
                    // 失能编码器NVIC中断
                    NVIC_DisableIRQ(GPIO_Encoder_INT_IRQN);
                    // 重置PID
                    PID_Reset(&PID_Motor_L);
                    PID_Reset(&PID_Motor_R);
                    // 停止PWM输出
                    DL_TimerG_stopCounter(PWM_MOTOR_INST);
                    // 失能定时器
                    NVIC_DisableIRQ(TIMER_Speed_INST_INT_IRQN);
                    DL_TimerG_stopCounter(TIMER_Speed_INST);
                    NVIC_DisableIRQ(TIMER_Motor_INST_INT_IRQN);
                    DL_TimerG_stopCounter(TIMER_Motor_INST);
                    NVIC_DisableIRQ(TIMER_Grayscale_Sensor_INST_INT_IRQN);
                    DL_TimerG_stopCounter(TIMER_Grayscale_Sensor_INST);
                    g_motor_running = 0;  /* 清除全局运行标志 */
                    g_blueserial_control_enable = 0;  /* 清除BlueSerial控制模式标志 */
                    g_target_speed_L = 0;  /* 清除目标速度 */
                    g_target_speed_R = 0;
                }
                if (!key2) key2_lock = 0;

                /* K1、K3在运行态无操作(避免误触) */

                /*-------------------------- 用户任务区 --------------------------*/
                /* 此处代码在运行态时每循环执行一次
                 * 中断中已处理: 编码器读取、电机PID计算、灰度采样
                 * 此处可添加显示刷新、调试输出等
                 */
                OLED_ShowNum(0, 6, distance * 10, 4, 16);  /* 显示灰度传感器偏差值 */

                /* 调试输出：显示左右电机完整信息 */
                BlueSerial_Printf("dist=%.1f steer=%.1f Speed:%f,%f\n", distance, PID_Grayscale_Sensor.output, Speed_R, Speed_L);
                BlueSerial_Printf("L: tar=%.0f spd=%.1f PID=%.1f dir=%d duty=%.2f | R: tar=%.0f spd=%.1f PID=%.1f dir=%d duty=%.2f\r\n",
                                  BASE_SPEED - PID_Grayscale_Sensor.output, Speed_L, PID_Motor_L.output, PWMCommand_L.direction, PWMCommand_L.duty,
                                  BASE_SPEED + PID_Grayscale_Sensor.output, Speed_R, PID_Motor_R.output, PWMCommand_R.direction, PWMCommand_R.duty);
                break;
        }
        /* switch结束后回到while循环继续检测按键 */
    }
}

/**
 * @brief Menu_Second_2 菜单函数
 *
 * 使用两状态非阻塞状态机实现启动/停止/退出控制
 *
 * 状态转换图:
 *   [停止态] --K1--> [运行态]
 *   [运行态] --K2--> [停止态]
 *   [停止态] --K3--> [退出]
 *
 * 按键说明:
 *   K1: 启动(停止态有效)
 *   K2: 停止(运行态有效)
 *   K3: 退出(仅在停止态有效)
 *
 * @return int 退出时返回0
 */
int Menu_Second_2(void)
{
    /*------------------------------ 变量声明 ------------------------------*/
    uint8_t state = MENU_STATE_STOP;     /* 状态机当前状态: 0=停止态 */

    /* 按键锁定标志位
     * 作用: 防止按键按住不放时同一动作被重复触发
     * 原理: 按下时锁定=1，此时忽略后续按下；松开后锁定清0，允许下次触发
     */
    uint8_t key1_lock = 0;  /* K1锁定标志 */
    uint8_t key2_lock = 0;  /* K2锁定标志 */
    uint8_t key3_lock = 0;  /* K3锁定标志 */

    /*------------------------------ 初始化显示 ------------------------------*/
    OLED_Clear();                                          /* 清屏 */
    OLED_ShowString(24, 0, (uint8_t *)"model_2", 16);    /* 显示标题 */
    OLED_ShowString(24, 2, (uint8_t *)"stop", 16);        /* 显示初始状态 */

    /*------------------------------ 主循环 ------------------------------*/
    while (1)
    {
        /*----------------------- 按键读取 -----------------------*/
        /* Key_GetNum() 返回1表示按键按下，返回0表示松开 */
        uint8_t key1 = Key1_GetNum();
        uint8_t key2 = Key2_GetNum();
        uint8_t key3 = Key3_GetNum();

        /*----------------------- 状态机处理 -----------------------*/
        switch (state)
        {
            /*========================= 停止态 =========================*/
            case MENU_STATE_STOP:
                /* 此状态下电机停止，等待用户操作 */

                /* K1启动: 按下K1且未锁定时进入运行态 */
                if (key1 && !key1_lock)
                {
                    key1_lock = 1;  /* 锁定K1，防止重复触发 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN); /* LED翻转提示 */
                    Buzzer_Delay_ON();  /* 蜂鸣器提示音 */
                    state = MENU_STATE_RUNNING;  /* 切换到运行态 */
                    OLED_ShowString(24, 2, (uint8_t *)"start", 16);  /* 更新显示状态 */

                    /*---------- 启动初始化代码(仅执行一次) ----------*/
                    // 开启速度测量定时器（先启动测速中断）
                    NVIC_EnableIRQ(TIMER_Speed_INST_INT_IRQN);
                    DL_TimerG_startCounter(TIMER_Speed_INST);
                    // 开启外设中断 + 启动定时器(电机PID计算)
                    NVIC_EnableIRQ(TIMER_Motor_INST_INT_IRQN);
                    DL_TimerG_startCounter(TIMER_Motor_INST);
                    g_motor_running = 1;  /* 设置全局运行标志 */
                    g_blueserial_control_enable = 1;  /* 默认BlueSerial控制模式 */
                    g_target_speed_L = 0;  /* 初始目标速度为0 */
                    g_target_speed_R = 0;
                }
                /* 按键松开后解除锁定，允许下次触发 */
                if (!key1) key1_lock = 0;

                /* K3退出: 按下K3且未锁定时退出菜单 */
                if (key3 && !key3_lock)
                {
                    key3_lock = 1;  /* 锁定K3 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);
                    Buzzer_Delay_ON();
                    OLED_Clear();  /* 退出前清屏 */
                    return 0;  /* 退出函数 */
                }
                if (!key3) key3_lock = 0;
                break;

            /*========================= 运行态 =========================*/
            case MENU_STATE_RUNNING:
                /* 此状态下执行用户任务 */

                /* K2停止: 按下K2且未锁定时返回停止态 */
                if (key2 && !key2_lock)
                {
                    key2_lock = 1;  /* 锁定K2 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);
                    Buzzer_Delay_ON();
                    state = MENU_STATE_STOP;  /* 切换到停止态 */
                    OLED_ShowString(24, 2, (uint8_t *)"stop", 16);  /* 更新显示状态 */

                    /*---------- 停止代码(仅执行一次) ----------*/
                    Motor_SetPWM(1, 0);
                    Motor_SetPWM(2, 0);
                    // 停止定时器
                    NVIC_DisableIRQ(TIMER_Speed_INST_INT_IRQN);
                    DL_TimerG_stopCounter(TIMER_Speed_INST);
                    NVIC_DisableIRQ(TIMER_Motor_INST_INT_IRQN);
                    DL_TimerG_stopCounter(TIMER_Motor_INST);
                    g_motor_running = 0;  /* 清除全局运行标志 */
                    g_blueserial_control_enable = 0;  /* 清除BlueSerial控制模式标志 */
                    g_target_speed_L = 0;  /* 清除目标速度 */
                    g_target_speed_R = 0;
                }
                if (!key2) key2_lock = 0;

                /* K1、K3在运行态无操作(避免误触) */

                /*-------------------------- 用户任务区 --------------------------*/
                /* 此处代码在运行态时每循环执行一次
                 * 例如: 蓝牙串口处理、传感器读取等
                 */
                BlueSerial_Tick(BlueSerial_RxPacket);
                break;
        }
        /* switch结束后回到while循环继续检测按键 */
    }
}

/**
 * @brief Menu_Second_3 菜单函数
 *
 * 使用两状态非阻塞状态机实现启动/停止/退出控制
 *
 * 状态转换图:
 *   [停止态] --K1--> [运行态]
 *   [运行态] --K2--> [停止态]
 *   [停止态] --K3--> [退出]
 *
 * 按键说明:
 *   K1: 启动(停止态有效)
 *   K2: 停止(运行态有效)
 *   K3: 退出(仅在停止态有效)
 *
 * @return int 退出时返回0
 */
int Menu_Second_3(void)
{
    /*------------------------------ 变量声明 ------------------------------*/
    uint8_t state = MENU_STATE_STOP;     /* 状态机当前状态: 0=停止态 */
    uint32_t counter = 0;                 /* 计数器: 记录运行时的计数 */

    /* 按键锁定标志位
     * 作用: 防止按键按住不放时同一动作被重复触发
     * 原理: 按下时锁定=1，此时忽略后续按下；松开后锁定清0，允许下次触发
     */
    uint8_t key1_lock = 0;  /* K1锁定标志 */
    uint8_t key2_lock = 0;  /* K2锁定标志 */
    uint8_t key3_lock = 0;  /* K3锁定标志 */

    /*------------------------------ 初始化显示 ------------------------------*/
    OLED_Clear();                                          /* 清屏 */
    OLED_ShowString(24, 0, (uint8_t *)"model_3", 16);     /* 显示标题 */
    OLED_ShowString(24, 2, (uint8_t *)"stop", 16);        /* 显示初始状态 */
    OLED_ShowNum(48, 4, counter, 5, 16);                  /* 显示计数器初始值 */

    /*------------------------------ 主循环 ------------------------------*/
    while (1)
    {
        /*----------------------- 按键读取 -----------------------*/
        /* Key_GetNum() 返回1表示按键按下，返回0表示松开 */
        uint8_t key1 = Key1_GetNum();
        uint8_t key2 = Key2_GetNum();
        uint8_t key3 = Key3_GetNum();

        /*----------------------- 状态机处理 -----------------------*/
        switch (state)
        {
            /*========================= 停止态 =========================*/
            case MENU_STATE_STOP:
                /* 此状态下计数器停止显示，等待用户操作 */

                /* K1启动: 按下K1且未锁定时进入运行态 */
                if (key1 && !key1_lock)
                {
                    key1_lock = 1;  /* 锁定K1，防止重复触发 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN); /* LED翻转提示 */
                    Buzzer_Delay_ON();  /* 蜂鸣器提示音 */
                    state = MENU_STATE_RUNNING;  /* 切换到运行态 */
                    OLED_ShowString(24, 2, (uint8_t *)"run ", 16);  /* 更新显示状态 */
                }
                /* 按键松开后解除锁定，允许下次触发 */
                if (!key1) key1_lock = 0;

                /* K3退出: 按下K3且未锁定时退出菜单 */
                if (key3 && !key3_lock)
                {
                    key3_lock = 1;  /* 锁定K3 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);
                    Buzzer_Delay_ON();
                    OLED_Clear();  /* 退出前清屏 */
                    return 0;  /* 退出函数 */
                }
                if (!key3) key3_lock = 0;
                break;

            /*========================= 运行态 =========================*/
            case MENU_STATE_RUNNING:
                /* 此状态下计数器递增显示，执行用户任务 */

                /* K2停止: 按下K2且未锁定时返回停止态 */
                if (key2 && !key2_lock)
                {
                    key2_lock = 1;  /* 锁定K2 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);
                    Buzzer_Delay_ON();
                    state = MENU_STATE_STOP;  /* 切换到停止态 */
                    OLED_ShowString(24, 2, (uint8_t *)"stop", 16);  /* 更新显示状态 */
                }
                if (!key2) key2_lock = 0;

                /* K1、K3在运行态无操作(避免误触) */

                /* 计数器递增并显示 */
                counter++;  /* 每循环一次计数器+1 */
                if (counter > 99999)  /* 5位数字上限，防止溢出 */
                {
                    counter = 0;  /* 归零循环 */
                }
                OLED_ShowNum(48, 4, counter, 5, 16);  /* 更新显示计数值 */

                /*-------------------------- 用户任务区 --------------------------*/
                /* 提示: 在此处添加需要循环执行的任务代码
                 * 例如: 电机控制、传感器读取、PID计算等
                 * 这些代码仅在运行态时执行
                 */
                break;
        }
        /* switch结束后回到while循环继续检测按键 */
    }
}

/**
 * @brief Menu_Second_4 菜单函数
 *
 * 使用两状态非阻塞状态机实现启动/停止/退出控制
 *
 * 状态转换图:
 *   [停止态] --K1--> [运行态]
 *   [运行态] --K2--> [停止态]
 *   [停止态] --K3--> [退出]
 *
 * 按键说明:
 *   K1: 启动(停止态有效)
 *   K2: 停止(运行态有效)
 *   K3: 退出(仅在停止态有效)
 *
 * @return int 退出时返回0
 */
int Menu_Second_4(void)
{
    /*------------------------------ 变量声明 ------------------------------*/
    uint8_t state = MENU_STATE_STOP;     /* 状态机当前状态: 0=停止态 */
    uint32_t counter = 0;                 /* 计数器: 记录运行时的计数 */

    /* 按键锁定标志位
     * 作用: 防止按键按住不放时同一动作被重复触发
     * 原理: 按下时锁定=1，此时忽略后续按下；松开后锁定清0，允许下次触发
     */
    uint8_t key1_lock = 0;  /* K1锁定标志 */
    uint8_t key2_lock = 0;  /* K2锁定标志 */
    uint8_t key3_lock = 0;  /* K3锁定标志 */

    /*------------------------------ 初始化显示 ------------------------------*/
    OLED_Clear();                                          /* 清屏 */
    OLED_ShowString(24, 0, (uint8_t *)"model_4", 16);     /* 显示标题 */
    OLED_ShowString(24, 2, (uint8_t *)"stop", 16);        /* 显示初始状态 */
    OLED_ShowNum(48, 4, counter, 5, 16);                  /* 显示计数器初始值 */

    /*------------------------------ 主循环 ------------------------------*/
    while (1)
    {
        /*----------------------- 按键读取 -----------------------*/
        /* Key_GetNum() 返回1表示按键按下，返回0表示松开 */
        uint8_t key1 = Key1_GetNum();
        uint8_t key2 = Key2_GetNum();
        uint8_t key3 = Key3_GetNum();

        /*----------------------- 状态机处理 -----------------------*/
        switch (state)
        {
            /*========================= 停止态 =========================*/
            case MENU_STATE_STOP:
                /* 此状态下计数器停止显示，等待用户操作 */

                /* K1启动: 按下K1且未锁定时进入运行态 */
                if (key1 && !key1_lock)
                {
                    key1_lock = 1;  /* 锁定K1，防止重复触发 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN); /* LED翻转提示 */
                    Buzzer_Delay_ON();  /* 蜂鸣器提示音 */
                    state = MENU_STATE_RUNNING;  /* 切换到运行态 */
                    OLED_ShowString(24, 2, (uint8_t *)"run ", 16);  /* 更新显示状态 */
                }
                /* 按键松开后解除锁定，允许下次触发 */
                if (!key1) key1_lock = 0;

                /* K3退出: 按下K3且未锁定时退出菜单 */
                if (key3 && !key3_lock)
                {
                    key3_lock = 1;  /* 锁定K3 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);
                    Buzzer_Delay_ON();
                    OLED_Clear();  /* 退出前清屏 */
                    return 0;  /* 退出函数 */
                }
                if (!key3) key3_lock = 0;
                break;

            /*========================= 运行态 =========================*/
            case MENU_STATE_RUNNING:
                /* 此状态下计数器递增显示，执行用户任务 */

                /* K2停止: 按下K2且未锁定时返回停止态 */
                if (key2 && !key2_lock)
                {
                    key2_lock = 1;  /* 锁定K2 */
                    DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_PIN_LED_PIN);
                    Buzzer_Delay_ON();
                    state = MENU_STATE_STOP;  /* 切换到停止态 */
                    OLED_ShowString(24, 2, (uint8_t *)"stop", 16);  /* 更新显示状态 */
                }
                if (!key2) key2_lock = 0;

                /* K1、K3在运行态无操作(避免误触) */

                /* 计数器递增并显示 */
                counter++;  /* 每循环一次计数器+1 */
                if (counter > 99999)  /* 5位数字上限，防止溢出 */
                {
                    counter = 0;  /* 归零循环 */
                }
                OLED_ShowNum(48, 4, counter, 5, 16);  /* 更新显示计数值 */

                /*-------------------------- 用户任务区 --------------------------*/
                /* 提示: 在此处添加需要循环执行的任务代码
                 * 例如: 电机控制、传感器读取、PID计算等
                 * 这些代码仅在运行态时执行
                 */
                break;
        }
        /* switch结束后回到while循环继续检测按键 */
    }
}


/**
 * @brief 编码器外部中断处理函数 (Group1)
 *
 * 触发条件：编码器GPIO引脚电平变化（外部中断）
 *
 * 功能说明：
 *   - 读取左右编码器的当前计数值
 *   - 通过编码器数据计算电机实际转速 (Speed_L, Speed_R)
 *
 * 执行条件：
 *   - 仅当 g_motor_running = 1 时执行
 *   - g_motor_running 由 Menu1/Menu2 运行时设置为1，停止时设置为0
 *
 * @note 此中断处理函数在Menu1（灰度循线）和Menu2（蓝牙控制）模式下均需要
 *       因为两个模式都需要编码器数据来计算电机实际转速
 */
void GROUP1_IRQHandler(void)
{
    /* 获取触发的中断组，并清除中断标志 */
    switch( DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1) )
    {
        /* 检查是否是编码器GPIO端口中断 */
        case GPIO_Encoder_INT_IIDX:
            if (g_motor_running != 0)  /* 只有在电机运行时才处理编码器 */
            {
                Encoder_Check();  /* 读取编码器并计算实际速度 */
            }
            break;

        default:
            /* 其他中断：不处理 */
            break;
    }
}

/**
 * @brief 电机控制定时器中断处理函数
 *
 * 触发条件：定时器计数达到周期值（定时器零匹配中断）
 *
 * 功能说明：
 *   - 执行电机PID控制计算
 *   - 输出PWM控制信号到电机驱动
 *
 * 双模式控制：
 *   - 当 g_blueserial_control_enable = 1（蓝牙模式）：使用 g_target_speed_L/R 作为目标速度
 *   - 当 g_blueserial_control_enable = 0（灰度模式）：使用 BASE_SPEED +/- 灰度PID输出 作为目标速度
 *
 * 执行条件：
 *   - 仅当 g_motor_running = 1 时执行
 *
 * 分频机制：
 *   - Count计数器每10次中断执行一次完整的PID计算和PWM输出
 *   - 这是为了匹配电机控制的时间常数，避免控制过于频繁
 *
 * @note PID计算使用增量式PID，输出PWM占空比和方向
 */
void TIMER_Motor_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_Motor_INST))
    {
        case DL_TIMER_IIDX_ZERO:  /* 定时器零匹配中断 */
            if (g_motor_running != 0)  /* 检查电机运行标志 */
            {
                /*-------- 根据控制模式计算目标速度 --------*/
                float target_L, target_R;  /* 左右轮目标速度 */
                float steering_adjust;      /* 灰度PID输出的转向校正量 */

                if (g_blueserial_control_enable)
                {
                    /* 蓝牙摇杆控制模式：使用摇杆设定的目标速度 */
                    target_L = g_target_speed_L;
                    target_R = g_target_speed_R;
                    steering_adjust = 0.0f;
                }
                else
                {
                    /* 灰度循线模式：
                     * 基础速度 BASE_SPEED 相同
                     * 转向通过 steering_adjust 调整左右轮速度差实现
                     *
                     * distance > 4.5: 黑线偏右，需要右转
                     *   -> 左轮加速（target_L 增大）
                     *   -> 右轮减速（target_R 减小）
                     * distance < 4.5: 黑线偏左，需要左转
                     *   -> 左轮减速（target_L 减小）
                     *   -> 右轮加速（target_R 增大）
                     *
                     * PID_Grayscale_Sensor.output > 0: distance < 4.5，左轮减速
                     * PID_Grayscale_Sensor.output < 0: distance > 4.5，右轮减速
                     */
                    steering_adjust = PID_Grayscale_Sensor.output;
                    /* distance < 4.5: 左转 -> 左轮慢右轮快 -> target_L减小, target_R增大
                     * distance > 4.5: 右转 -> 左轮快右轮慢 -> target_L增大, target_R减小
                     * steering > 0 (distance<4.5): target_L减小, target_R增大
                     * steering < 0 (distance>4.5): target_L增大, target_R减小
                     */
                    target_L = BASE_SPEED - steering_adjust;  /* steering>0时左轮减速 */
                    target_R = BASE_SPEED + steering_adjust;  /* steering>0时右轮加速 */
                }

                /*-------- 执行PID计算，输出PWM占空比 --------*/
                PID_UpdateToPWM(&PID_Motor_L, target_L, Speed_L,
                                PWM_MIN_DUTY, PWM_MAX_DUTY, &PWMCommand_L);
                PID_UpdateToPWM(&PID_Motor_R, target_R, Speed_R,
                                PWM_MIN_DUTY, PWM_MAX_DUTY, &PWMCommand_R);

                /*-------- 输出PWM到电机驱动 --------*/
                /* 注意：PWMCommand.duty 可能是正数或负数
                 * 正数 -> Motor_SetPWM 解释为正转（INx引脚设正转方向）
                 * 负数 -> Motor_SetPWM 解释为反转（INx引脚设反转方向）
                 */
                Motor_SetPWM(1, PWMCommand_L.duty);  /* 左电机PWM */
                Motor_SetPWM(2, PWMCommand_R.duty);  /* 右电机PWM */

            }
            break;

        default:
            /* 其他定时器中断：不处理 */
            break;
    }
}
/**
 * @brief 八路灰度传感器定时器中断处理函数
 *
 * 触发条件：灰度传感器采样定时器计数达到周期值
 *
 * 功能说明：
 *   - 采样八路灰度传感器数据
 *   - 执行位置式PID计算，输出转向控制量
 *
 * 执行条件：
 *   - 仅当 g_motor_running = 1 时执行
 *
 * PID说明：
 *   - 使用位置式PID控制器
 *   - 目标值：4.5（表示小车应该在第4和第5个传感器之间，即黑线中心）
 *   - 反馈值：distance（灰度传感器检测到的黑线位置）
 *   - 输出：转向校正量（负值=左转，正值=右转）
 *
 * @note 此中断仅在Menu1（灰度循线模式）下启用
 */
void TIMER_Grayscale_Sensor_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_Grayscale_Sensor_INST))
    {
        case DL_TIMER_IIDX_ZERO:  /* 定时器零匹配中断 */
            if (g_motor_running != 0)  /* 检查电机运行标志 */
            {
                /* 读取八路灰度传感器数据，更新distance变量 */
                Graysccale_Sensor_PinCheck();

                /* 执行位置式PID计算
                 * 目标位置：4.5（两个传感器中间，黑线中心对齐）
                 * 反馈位置：distance（当前检测到的黑线位置）
                 * 输出：转向校正量
                 */
                PID_Positional_Update(&PID_Grayscale_Sensor, 4.5, distance);
            }
            break;

        default:
            /* 其他定时器中断：不处理 */
            break;
    }
}

/**
 * @brief 速度测量定时器中断处理函数
 *
 * 触发条件：定时器计数达到周期值（定时器零匹配中断）
 *
 * 功能说明：
 *   - 读取左右编码器当前计数值
 *   - 计算两次采样之间的脉冲差（带跨零溢出修正）
 *   - 将脉冲差转换为轮子移动距离，计算实际速度（mm/s）
 *
 * 执行条件：
 *   - 仅当 g_motor_running = 1 时执行
 *
 * 速度计算公式：
 *   - 速度 = (脉冲差 / 每转脉冲数) × 轮子周长 × 采样频率
 *
 * @see Motor_Speed() 速度计算具体实现
 */
void TIMER_Speed_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_Speed_INST))
    {
        case DL_TIMER_IIDX_ZERO:  /* 定时器零匹配中断 */
            if (g_motor_running != 0)  /* 检查电机运行标志 */
            {
                /*-------- 读取编码器计算实际速度 --------*/
                Motor_Speed();
            }
            break;

        default:
            /* 其他定时器中断：不处理 */
            break;
    }
}
