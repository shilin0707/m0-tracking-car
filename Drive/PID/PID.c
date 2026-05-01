/**
 * @file PID.c
 * @brief PID控制器源文件
 *
 * 本文件实现了两种PID控制器算法：
 *
 * 1. 增量式PID (PID_Controller)
 *    - 用途：电机速度闭环控制
 *    - 特点：
 *      * 计算控制增量Δu(k)，而不是绝对量
 *      * 使用一阶滤波减少噪声影响
 *      * 使用上上次误差e(k-2)进行微分项计算
 *
 * 2. 位置式PID (PID_Positional_Controller)
 *    - 用途：灰度传感器循线转向控制
 *    - 特点：
 *      * 直接计算绝对控制量u(k)
 *      * 微分项使用一阶滤波
 *      * 包含积分项消除稳态误差
 *
 * 算法选择依据：
 *   - 增量式PID适合执行机构是积分累加型的场景（如PWM积分成电压）
 *   - 位置式PID适合执行机构直接是位置/角度型的场景
 */

#include "PID.h"

/*================================== 辅助函数 =====================================*/

/**
 * @brief 限幅辅助函数
 *
 * @param value 待限幅的值
 * @param min_value 下限
 * @param max_value 上限
 * @return 限幅后的值
 *
 * @note 如果value超过[min,max]范围，返回边界值
 */
static float PID_Clamp(float value, float min_value, float max_value)
{
    if (value > max_value)
    {
        return max_value;
    }
    if (value < min_value)
    {
        return min_value;
    }
    return value;
}

/*================================== 增量式PID实现 =====================================*/

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
 *
 * @note 内部调用 PID_Reset 初始化状态变量
 */
void PID_Init(PID_Controller *pid,
              float kp,
              float ki,
              float kd,
              float dt,
              float filter_alpha,
              float dead_zone,
              float max_speed,
              float integral_limit)
{
    if (pid == 0)
    {
        return;
    }

    /* 赋值PID系数 */
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    /* 设置控制参数（带安全检查）*/
    pid->dt = (dt > 0.0f) ? dt : 0.001f;  /* 默认1ms */
    pid->filter_alpha = PID_Clamp(filter_alpha, 0.0f, 1.0f);  /* 限幅到[0,1] */
    pid->dead_zone = (dead_zone >= 0.0f) ? dead_zone : 0.0f;  /* 死区非负 */

    pid->max_speed = (max_speed > 0.0f) ? max_speed : 0.0f;  /* 最大速度非负 */
    pid->integral_limit = (integral_limit > 0.0f) ? integral_limit : pid->max_speed;  /* 默认等于最大速度 */

    PID_Reset(pid);  /* 初始化状态变量 */
}

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
                    float integral_limit)
{
    PID_Init(pid,
             PID_PRESET_KP,
             PID_PRESET_KI,
             PID_PRESET_KD,
             dt,
             filter_alpha,
             dead_zone,
             max_speed,
             integral_limit);
}

/**
 * @brief 增量式PID重置
 *
 * @param pid PID控制器结构体指针
 *
 * 功能说明：
 *   - 将所有状态变量清零
 *   - 设置首次运行标志 first = 1
 *
 * @note 调用此函数后，下次调用 PID_Update 会进行滤波初始化
 */
void PID_Reset(PID_Controller *pid)
{
    if (pid == 0)
    {
        return;
    }

    pid->err = 0.0f;                   /* 当前误差清零 */
    pid->prev_filtered_err = 0.0f;      /* 上次滤波误差 e(k-1) = 0 */
    pid->prev_prev_filtered_err = 0.0f; /* 上上次滤波误差 e(k-2) = 0 */
    pid->integral = 0.0f;              /* 积分项（保留，兼容）*/
    pid->derivative = 0.0f;             /* 微分项（存储Δu(k)）*/
    pid->output = 0.0f;                 /* 输出清零 */
    pid->first = 1;                     /* 首次运行标志置1 */
}

/**
 * @brief 增量式PID更新计算
 *
 * @param pid PID控制器结构体指针
 * @param target_speed 目标速度
 * @param measured_speed 测量速度（实际速度）
 * @return PID控制输出（速度值）
 *
 * 算法说明：
 *   增量式PID计算控制增量Δu(k)：
 *   Δu(k) = Kp[e(k)-e(k-1)] + Ki·dt·e(k) + Kd[e(k)-2e(k-1)+e(k-2)]/dt
 *
 * 一阶滤波：
 *   首次运行时，filtered_err = 0.4 * raw_err（避免初始跳变）
 *   之后：filtered_err = α * raw_err + (1-α) * prev_filtered_err
 *
 * @note 用于电机速度闭环控制
 */
float PID_Update(PID_Controller *pid, float target_speed, float measured_speed)
{
    float raw_err;           /* 原始误差 */
    float filtered_err;       /* 滤波后误差 */
    float delta_u;            /* 控制增量 Δu(k) */
    float e_k, e_k1, e_k2;   /* e(k)、e(k-1)、e(k-2) */

    if (pid == 0)
    {
        return 0.0f;
    }

    /* 目标速度限幅 */
    target_speed = PID_Clamp(target_speed, -pid->max_speed, pid->max_speed);

    /* 计算原始误差：目标 - 测量 */
    raw_err = target_speed - measured_speed;

    /* 误差死区处理 */
    if (fabsf(raw_err) < pid->dead_zone)
    {
        raw_err = 0.0f;
    }

    /*-------- 一阶滤波 --------*/
    /**
     * 首次运行时使用较大权重初始化，避免初始跳变
     * 之后使用正常的一阶滤波公式
     */
    switch (pid->first)
    {
        case 1:
            /* 首次运行：使用0.4权重初始化 */
            filtered_err = 0.4f * raw_err;
            pid->first = 0;  /* 清除首次标志 */
            break;

        case 0:
            /* 正常滤波：filtered = α * current + (1-α) * last */
            filtered_err = pid->filter_alpha * raw_err +
                          (1.0f - pid->filter_alpha) * pid->prev_filtered_err;
            break;
    }

    /*-------- 增量式PID核心计算 --------*/
    /**
     * 增量式PID公式：
     * Δu(k) = Kp[e(k)-e(k-1)] + Ki·dt·e(k) + Kd[e(k)-2e(k-1)+e(k-2)]/dt
     *
     * 第一项：比例作用，响应误差变化
     * 第二项：积分作用，消除稳态误差
     * 第三项：微分作用，预测误差趋势
     */
    e_k  = filtered_err;                  /* 本次滤波误差 e(k) */
    e_k1 = pid->prev_filtered_err;        /* 上次滤波误差 e(k-1) */
    e_k2 = pid->prev_prev_filtered_err;   /* 上上次滤波误差 e(k-2) */

    /* 计算控制增量 */
    delta_u = pid->kp * (e_k - e_k1) +                   /* 比例项 */
              pid->ki * pid->dt * e_k +                  /* 积分项 */
              pid->kd * (e_k - 2.0f * e_k1 + e_k2) / pid->dt;  /* 微分项 */

    /* 累积得到当前输出：u(k) = u(k-1) + Δu(k) */
    pid->output += delta_u;

    /*-------- 输出限幅 --------*/
    pid->output = PID_Clamp(pid->output, -pid->max_speed, pid->max_speed);

    /*-------- 更新历史误差（供下一次计算）--------*/
    pid->err = raw_err;  /* 保存原始误差 */
    pid->prev_prev_filtered_err = e_k1;  /* e(k-2) = 原e(k-1) */
    pid->prev_filtered_err = e_k;        /* e(k-1) = 本次e(k) */
    pid->derivative = delta_u;           /* 保存本次增量（兼容原字段）*/

    return pid->output;
}

/*================================== PWM转换函数 =====================================*/

/**
 * @brief PID输出转换为PWM命令
 *
 * @param pid PID控制器指针
 * @param pid_output PID输出值
 * @param pwm_min_duty PWM最小占空比
 * @param pwm_max_duty PWM最大占空比
 * @param cmd PWM命令输出结构体指针
 *
 * 功能说明：
 *   - 根据PID输出的符号确定方向（正=前进，负=后退）
 *   - 根据PID输出的绝对值计算占空比
 *   - 占空比在[pwm_min_duty, pwm_max_duty]范围内线性映射
 *
 * @note pwm_min_duty通常设置为克服静摩擦的最小占空比
 */
void PID_OutputToPWM(const PID_Controller *pid,
                     float pid_output,
                     float pwm_min_duty,
                     float pwm_max_duty,
                     PID_PWMCommand *cmd)
{
    float output_abs;   /* PID输出的绝对值 */
    float normalized;   /* 归一化后的值 [0,1] */

    if ((pid == 0) || (cmd == 0))
    {
        return;
    }

    /* 参数有效性检查 */
    if ((pid->max_speed <= 0.0f) || (pwm_max_duty <= 0.0f) || (pwm_max_duty <= pwm_min_duty))
    {
        cmd->direction = 0;
        cmd->duty = 0.0f;
        return;
    }

    /* PID输出限幅 */
    pid_output = PID_Clamp(pid_output, -pid->max_speed, pid->max_speed);

    /* 死区检查 */
    if (fabsf(pid_output) < pid->dead_zone)
    {
        cmd->direction = 0;
        cmd->duty = 0.0f;
        return;
    }

    /* 确定方向：正数=正转，负数=反转 */
    cmd->direction = (pid_output > 0.0f) ? 1 : -1;

    /* 计算占空比（保留pid_output的符号） */
    output_abs = fabsf(pid_output);
    normalized = PID_Clamp(output_abs / pid->max_speed, 0.0f, 1.0f);  /* 归一化到[0,1] */

    /* 线性映射到PWM范围，然后加上符号 */
    cmd->duty = pwm_min_duty + normalized * (pwm_max_duty - pwm_min_duty);
    cmd->duty = PID_Clamp(cmd->duty, 0.0f, pwm_max_duty);  /* 先限幅成正数 */
    if (cmd->direction > 0) { cmd->duty = fabs(cmd->duty); }
    else if (cmd->direction < 0) { cmd->duty = -cmd->duty; }
}

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
                     PID_PWMCommand *cmd)
{
    float pid_output;

    if ((pid == 0) || (cmd == 0))
    {
        return;
    }

    /* 先计算PID输出 */
    pid_output = PID_Update(pid, target_speed, measured_speed);

    /* 转换为PWM命令 */
    PID_OutputToPWM(pid, pid_output, pwm_min_duty, pwm_max_duty, cmd);
}

/*================================== 位置式PID实现 =====================================*/

/**
 * @brief 限幅辅助函数（位置式PID专用）
 *
 * @param value 待限幅的值
 * @param limit 限幅阈值（正负对称）
 * @return 限幅后的值
 *
 * @note 与PID_Clamp的区别：使用对称限幅
 */
static float Positional_PID_Clamp(float value, float limit)
{
    if (value > limit)
    {
        return limit;
    }
    if (value < -limit)
    {
        return -limit;
    }
    return value;
}

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
 *
 * @note 内部调用 PID_Positional_Reset 初始化状态变量
 */
void PID_Positional_Init(PID_Positional_Controller *pid,
                         float kp,
                         float ki,
                         float kd,
                         float dt,
                         float integral_limit,
                         float derivative_filter,
                         float dead_zone,
                         float output_limit)
{
    if (pid == 0)
    {
        return;
    }

    /* 赋值PID系数 */
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    /* 设置控制参数（带安全检查）*/
    pid->dt = (dt > 0.0f) ? dt : 0.001f;  /* 默认1ms */
    pid->integral_limit = (integral_limit > 0.0f) ? integral_limit : 0.0f;
    pid->derivative_filter = (derivative_filter >= 0.0f && derivative_filter <= 1.0f) ?
                             derivative_filter : 0.0f;
    pid->dead_zone = (dead_zone >= 0.0f) ? dead_zone : 0.0f;
    pid->output_limit = (output_limit > 0.0f) ? output_limit : 0.0f;

    PID_Positional_Reset(pid);  /* 初始化状态变量 */
}

/**
 * @brief 位置式PID重置
 *
 * @param pid PID控制器结构体指针
 *
 * 功能说明：
 *   - 将所有状态变量清零
 *
 * @note 位置式PID的重置相对简单，只需清零各状态变量
 */
void PID_Positional_Reset(PID_Positional_Controller *pid)
{
    if (pid == 0)
    {
        return;
    }

    pid->err = 0.0f;
    pid->prev_err = 0.0f;
    pid->prev_derivative = 0.0f;
    pid->integral = 0.0f;
    pid->output = 0.0f;
}

/**
 * @brief 位置式PID更新计算
 *
 * @param pid PID控制器结构体指针
 * @param target 目标值
 * @param measured 测量值（当前值）
 * @return PID控制输出
 *
 * 位置式PID公式：
 *   u(k) = Kp·e(k) + Ki·Σe(k) + Kd·(e(k) - e(k-1))/dt
 *
 * 各分项说明：
 *   - P（比例）：与当前误差成正比，响应速度快
 *   - I（积分）：累积历史误差，消除稳态误差
 *   - D（微分）：误差变化率，抑制超调
 *
 * 微分项一阶滤波：
 *   - filtered_D = α × current_D + (1-α) × prev_D
 *   - α越大，对噪声越敏感；α越小，滤波效果越强
 *
 * @note 用于灰度传感器循线转向控制
 *       目标值通常设为4.5（两个传感器中间位置）
 */
float PID_Positional_Update(PID_Positional_Controller *pid, float target, float measured)
{
    float err;                   /* 当前误差 e(k) */
    float proportional;          /* 比例项 */
    float integral;              /* 积分项 */
    float derivative;           /* 微分项（未滤波）*/
    float filtered_derivative;   /* 微分项（滤波后）*/
    float raw_output;            /* 原始输出（限幅前）*/

    if (pid == 0)
    {
        return 0.0f;
    }

    /* 计算当前误差：目标值 - 测量值 */
    err = target - measured;

    /* 误差死区处理 */
    if (fabsf(err) < pid->dead_zone)
    {
        err = 0.0f;
    }

    /*-------- 比例项 P --------*/
    /**
     * P = Kp × e(k)
     * 直接与当前误差成正比，响应速度快
     */
    proportional = pid->kp * err;

    /*-------- 积分项 I --------*/
    /**
     * I = Ki × Σe(k) × dt
     * 累积历史误差，消除稳态误差（如摩擦力导致的恒定偏差）
     * 积分限幅防止积分饱和导致系统震荡
     */
    pid->integral += pid->ki * pid->dt * err;  /* 累积积分 */
    pid->integral = Positional_PID_Clamp(pid->integral, pid->integral_limit);  /* 限幅 */
    integral = pid->integral;

    /*-------- 微分项 D --------*/
    /**
     * D = Kd × (e(k) - e(k-1)) / dt
     * 误差变化率，预测系统趋势，抑制超调
     * 使用一阶滤波减少高频噪声影响
     */
    derivative = pid->kd * (err - pid->prev_err) / pid->dt;  /* 微分（未滤波）*/
    filtered_derivative = pid->derivative_filter * derivative +
                         (1.0f - pid->derivative_filter) * pid->prev_derivative;  /* 一阶滤波 */

    /* 计算位置式PID原始输出 */
    raw_output = proportional + integral + filtered_derivative;

    /* 输出限幅 */
    pid->output = Positional_PID_Clamp(raw_output, pid->output_limit);

    /* 更新历史误差，供下一次计算使用 */
    pid->err = err;
    pid->prev_err = err;  /* 保存本次误差作为下一次的上次误差 */
    pid->prev_derivative = filtered_derivative;  /* 保存滤波后微分项 */

    return pid->output;
}
