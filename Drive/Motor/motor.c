/**
 * @file motor.c
 * @brief 直流电机驱动源文件
 *
 * 硬件说明：
 *   - 电机驱动芯片：TB6612FNG（或类似双H桥驱动）
 *   - 控制方式：PWM调速 + IN1/IN2方向控制
 *   - 编码器：AB相增量式编码器，用于测速
 *
 * 速度计算：
 *   - 编码器每转脉冲数：CIRCLE = 728
 *   - 轮子直径：DIAMETER = 65mm
 *   - 速度 = (脉冲差 / CIRCLE) * π * 直径 * 采样频率
 *
 * 编码器处理：
 *   - 使用跨零修正处理编码器计数溢出
 *   - 使用一阶滤波平滑速度信号
 */

#include "motor.h"

/*================================== 全局变量 =====================================*/
/**
 * @brief 编码器当前计数值
 * @note 由编码器中断或读取函数更新
 */
int32_t Current_L = 0;   /**< 左编码器当前计数值 */
int32_t Current_R = 0;   /**< 右编码器当前计数值 */

int32_t Previous_L = 0;  /**< 左编码器上一次计数值 */
int32_t Previous_R = 0;  /**< 右编码器上一次计数值 */

float Speed_L = 0;      /**< 左轮实际速度，单位：mm/s */
float Speed_R = 0;      /**< 右轮实际速度，单位：mm/s */

/*================================== 电机方向控制函数 =====================================*/

/**
 * @brief 设置电机A方向控制引脚IN1为高电平
 * @note 电机A为右轮
 */
void Set_A_IN1(void)
{
    DL_GPIO_setPins(GPIO_Motor_PORT, GPIO_Motor_A_IN1_PIN);
}

/**
 * @brief 设置电机A方向控制引脚IN1为低电平
 */
void Clear_A_IN1(void)
{
    DL_GPIO_clearPins(GPIO_Motor_PORT, GPIO_Motor_A_IN1_PIN);
}

/**
 * @brief 设置电机A方向控制引脚IN2为高电平
 */
void Set_A_IN2(void)
{
    DL_GPIO_setPins(GPIO_Motor_PORT, GPIO_Motor_A_IN2_PIN);
}

/**
 * @brief 设置电机A方向控制引脚IN2为低电平
 */
void Clear_A_IN2(void)
{
    DL_GPIO_clearPins(GPIO_Motor_PORT, GPIO_Motor_A_IN2_PIN);
}

/**
 * @brief 设置电机B方向控制引脚IN1为高电平
 * @note 电机B为左轮
 */
void Set_B_IN1(void)
{
    DL_GPIO_setPins(GPIO_Motor_PORT, GPIO_Motor_B_IN1_PIN);
}

/**
 * @brief 设置电机B方向控制引脚IN1为低电平
 */
void Clear_B_IN1(void)
{
    DL_GPIO_clearPins(GPIO_Motor_PORT, GPIO_Motor_B_IN1_PIN);
}

/**
 * @brief 设置电机B方向控制引脚IN2为高电平
 */
void Set_B_IN2(void)
{
    DL_GPIO_setPins(GPIO_Motor_PORT, GPIO_Motor_B_IN2_PIN);
}

/**
 * @brief 设置电机B方向控制引脚IN2为低电平
 */
void Clear_B_IN2(void)
{
    DL_GPIO_clearPins(GPIO_Motor_PORT, GPIO_Motor_B_IN2_PIN);
}

/*================================== 电机PWM控制函数 =====================================*/

/**
 * @brief 直流电机设置PWM
 *
 * @param n 电机编号：1=左轮(B电机)，2=右轮(A电机)
 * @param Duty PWM占空比，范围：0~1（正数），负数表示反转
 *
 * 方向控制逻辑（TB6612FNG）：
 *   - IN1=1, IN2=0 -> 正转
 *   - IN1=0, IN2=1 -> 反转
 *   - IN1=0, IN2=0 -> 制动（停止）
 *
 * @note PWM占空比 Duty 范围是 0.0~1.0，对应 0%~100%
 */
void Motor_SetPWM(uint8_t n, float Duty)
{
    if (n == 1)  /* 左轮 - B电机 */
    {
        if (Duty >= 0)  /* 正转 */
        {
            Set_B_IN2();           /* IN2=1 */
            Clear_B_IN1();          /* IN1=0 -> 正转 */
            Set_Duty(1, Duty);      /* 设置PWM占空比 */
        }
        else  /* 反转 */
        {
            Set_B_IN1();           /* IN1=1 */
            Clear_B_IN2();          /* IN2=0 -> 反转 */
            Set_Duty(1, -Duty);     /* PWM取绝对值 */
        }
    }
    else if (n == 2)  /* 右轮 - A电机 */
    {
        if (Duty >= 0)  /* 正转 */
        {
            Set_A_IN1();           /* IN1=1 */
            Clear_A_IN2();          /* IN2=0 -> 正转 */
            Set_Duty(2, Duty);      /* 设置PWM占空比 */
        }
        else  /* 反转 */
        {
            Set_A_IN2();           /* IN2=1 */
            Clear_A_IN1();          /* IN1=0 -> 反转 */
            Set_Duty(2, -Duty);    /* PWM取绝对值 */
        }
    }
}

/*================================== 速度计算函数 =====================================*/

/**
 * @brief 电机速度计算
 *
 * 功能说明：
 *   - 读取编码器当前计数值
 *   - 计算两次采样之间的脉冲差值
 *   - 进行跨零修正（处理编码器溢出）
 *   - 将脉冲差转换为实际速度（mm/s）
 *   - 进行一阶滤波平滑速度信号
 *
 * 速度计算公式：
 *   - 轮子周长 = π × 直径 = π × 65mm
 *   - 每脉冲移动距离 = 周长 / 每转脉冲数 = (π × 65) / 728 mm
 *   - 速度 = 每脉冲移动距离 × 脉冲差 × 采样频率
 *
 * 跨零修正说明：
 *   - 编码器计数器最大值为1073741827，溢出后回绕
 *   - 当差值超过 half_max 时，说明发生了溢出回绕
 *   - 例如：差值 = +800000000，实际应为 -273741827
 *
 * 一阶滤波说明：
 *   - filtered = K × current + (1-K) × last
 *   - K越大，对瞬时值跟随越快；K越小，滤波效果越平滑
 */
void Motor_Speed(void)
{
    /* 保存上一次的编码器计数值 */
    Previous_L = Current_L;
    Current_L = Encoder_L;  /* 读取左编码器当前值 */

    Previous_R = Current_R;
    Current_R = Encoder_R;  /* 读取右编码器当前值 */

    /* 计算两次采样之间的编码器差值 */
    int16_t Variation_L = Current_L - Previous_L;  /* 左轮脉冲差 */
    int16_t Variation_R = Current_R - Previous_R;  /* 右轮脉冲差 */

    /* 计算半轮转的计数值（用于跨零判定）*/
    uint32_t half_max = ENCODER_OVERFLOW_MAX / 2;

    /*-------- 跨零修正：处理编码器计数溢出 --------*/
    /**
     * 当差值超过 half_max 时，说明编码器从最大值回绕到很小值
     * 此时应该用 (ENCODER_OVERFLOW_MAX - 当前值) + 上一次值 来计算真实差值
     */
    if (Variation_L > (int32_t)half_max)
    {
        /* 正向溢出：计数器从高值回绕到低值 */
        Variation_L -= (int32_t)ENCODER_OVERFLOW_MAX;
    }
    else if (Variation_L < -(int32_t)half_max)
    {
        /* 负向溢出：计数器从低值跳到高值 */
        Variation_L += (int32_t)ENCODER_OVERFLOW_MAX;
    }

    if (Variation_R > (int32_t)half_max)
    {
        Variation_R -= (int32_t)ENCODER_OVERFLOW_MAX;
    }
    else if (Variation_R < -(int32_t)half_max)
    {
        Variation_R += (int32_t)ENCODER_OVERFLOW_MAX;
    }

    /*-------- 将脉冲差转换为轮子移动距离（mm）--------*/
    /**
     * 公式：距离 = (脉冲差 / 每转脉冲数) × 轮子周长
     *     = (脉冲差 / 728) × π × 65
     */
    float Length_L = (float)(Variation_L / CIRCLE) * PI * DIAMETER;
    float Length_R = (float)(Variation_R / CIRCLE) * PI * DIAMETER;

    /*-------- 一阶滤波：平滑速度信号 --------*/
    static float last_val_L, filtered_val_L;   /* 左轮滤波变量 */
    static float last_val_R, filtered_val_R;   /* 右轮滤波变量 */

    /**
     * 一阶低通滤波公式：
     * filtered = K × current + (1-K) × last
     * K=0.2：当前值占20%，历史值占80%
     */
    filtered_val_L = K * Length_L + (1 - K) * last_val_L;
    last_val_L = filtered_val_L;

    filtered_val_R = K * Length_R + (1 - K) * last_val_R;
    last_val_R = filtered_val_R;

    /*-------- 计算实际速度（mm/s）--------*/
    /**
     * 速度 = 移动距离 × 采样频率
     * TIM_FRE = 100Hz，即每10ms采样一次
     * 所以速度单位为 mm/s
     */
    Speed_L = filtered_val_L * TIM_FRE;
    Speed_R = filtered_val_R * TIM_FRE;
}
