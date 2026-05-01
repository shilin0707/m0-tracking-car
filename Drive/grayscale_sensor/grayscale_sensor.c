/**
 * @file grayscale_sensor.c
 * @brief 八路灰度传感器驱动源文件
 *
 * 工作原理：
 *   - 红外发射管发射红外光
 *   - 黑色表面吸收红外光，反射少
 *   - 白色表面反射红外光，反射多
 *   - 通过比较器输出数字量（0或1）
 *
 * 循线原理：
 *   - 黑线位置用加权平均计算：1~8
 *   - 目标值4.5表示小车居中
 *   - 位置<4.5表示偏左，需要右转校正
 *   - 位置>4.5表示偏右，需要左转校正
 */

#include "grayscale_sensor.h"

/*================================== 全局变量 =====================================*/
float distance;           /**< 当前黑线位置（加权平均计算结果）*/
float last_distancce;     /**< 上一次有效的黑线位置（用于异常处理）*/

/*================================== 传感器读取函数 =====================================*/
/**
 * @brief 读取左4灰度传感器
 * @return int 传感器输出电平（0或1）
 */
int Graysccale_Sensor_Read_L_1(void)
{
    return DL_GPIO_readPins(GPIO_Grayscale_Sensor_PIN_L_1_PORT,
                            GPIO_Grayscale_Sensor_PIN_L_1_PIN);
}

int Graysccale_Sensor_Read_L_2(void)
{
    return DL_GPIO_readPins(GPIO_Grayscale_Sensor_PIN_L_2_PORT,
                            GPIO_Grayscale_Sensor_PIN_L_2_PIN);
}

int Graysccale_Sensor_Read_L_3(void)
{
    return DL_GPIO_readPins(GPIO_Grayscale_Sensor_PIN_L_3_PORT,
                            GPIO_Grayscale_Sensor_PIN_L_3_PIN);
}

int Graysccale_Sensor_Read_L_4(void)
{
    return DL_GPIO_readPins(GPIO_Grayscale_Sensor_PIN_L_4_PORT,
                            GPIO_Grayscale_Sensor_PIN_L_4_PIN);
}

/**
 * @brief 读取右4灰度传感器
 * @return int 传感器输出电平（0或1）
 */
int Graysccale_Sensor_Read_R_1(void)
{
    return DL_GPIO_readPins(GPIO_Grayscale_Sensor_PIN_R_1_PORT,
                            GPIO_Grayscale_Sensor_PIN_R_1_PIN);
}

int Graysccale_Sensor_Read_R_2(void)
{
    return DL_GPIO_readPins(GPIO_Grayscale_Sensor_PIN_R_2_PORT,
                            GPIO_Grayscale_Sensor_PIN_R_2_PIN);
}

int Graysccale_Sensor_Read_R_3(void)
{
    return DL_GPIO_readPins(GPIO_Grayscale_Sensor_PIN_R_3_PORT,
                            GPIO_Grayscale_Sensor_PIN_R_3_PIN);
}

int Graysccale_Sensor_Read_R_4(void)
{
    return DL_GPIO_readPins(GPIO_Grayscale_Sensor_PIN_R_4_PORT,
                            GPIO_Grayscale_Sensor_PIN_R_4_PIN);
}

/*================================== 采样与位置计算 =====================================*/

/**
 * @brief 灰度传感器采样与黑线位置计算
 *
 * 功能说明：
 *   1. 读取8个灰度传感器的数字输出
 *   2. 统计检测到黑线的传感器数量
 *   3. 使用加权平均法计算黑线中心位置
 *   4. 处理异常情况（全黑或全白）
 *
 * 传感器编号与位置：
 *   L4(1) - L3(2) - L2(3) - L1(4) - R1(5) - R2(6) - R3(7) - R4(8)
 *
 * 加权平均公式：
 *   distance = Σ(编号 × 传感器值) / 检测数量
 *   例：L1=1,R1=1（中间两个传感器检测到黑线）
 *       distance = (4×1 + 5×1) / 2 = 4.5（正好居中）
 *
 * 异常处理：
 *   - 所有传感器都未检测到黑线(number=0)时，number=1避免除零
 *   - 当计算出的distance=0时，保留上次的有效值
 *
 * @note 由灰度传感器定时器定时调用，更新distance全局变量
 */
void Graysccale_Sensor_PinCheck(void)
{
    /* 传感器状态变量：0=未检测到黑线，1=检测到黑线 */
    uint8_t L1 = 0, L2 = 0, L3 = 0, L4 = 0,
            R1 = 0, R2 = 0, R3 = 0, R4 = 0;
    float number = 0;  /* 检测到黑线的传感器数量 */

    /* 读取8个传感器状态 */
    if (Graysccale_Sensor_Read_L_4() != 0) {L4 = 1; number++;} else {L4 = 0;}
    if (Graysccale_Sensor_Read_L_3() != 0) {L3 = 1; number++;} else {L3 = 0;}
    if (Graysccale_Sensor_Read_L_2() != 0) {L2 = 1; number++;} else {L2 = 0;}
    if (Graysccale_Sensor_Read_L_1() != 0) {L1 = 1; number++;} else {L1 = 0;}
    if (Graysccale_Sensor_Read_R_1() != 0) {R1 = 1; number++;} else {R1 = 0;}
    if (Graysccale_Sensor_Read_R_2() != 0) {R2 = 1; number++;} else {R2 = 0;}
    if (Graysccale_Sensor_Read_R_3() != 0) {R3 = 1; number++;} else {R3 = 0;}
    if (Graysccale_Sensor_Read_R_4() != 0) {R4 = 1; number++;} else {R4 = 0;}

    /* 防止除零：所有传感器都未检测到黑线时 */
    if (number == 0)
        number = 1;  /* 设为1，后续计算distance会为0，触发保留上次值逻辑 */

    /*-------- 加权平均计算黑线位置 --------*/
    /**
     * 位置 = Σ(传感器编号 × 传感器值) / 检测数量
     *
     * 传感器编号（从左到右）：
     * L4=1, L3=2, L2=3, L1=4, R1=5, R2=6, R3=7, R4=8
     *
     * 例1：小车居中，position = 4.5
     *     L1=1, R1=1 (检测到黑线)
     *     = (4×1 + 5×1) / 2 = 4.5
     *
     * 例2：小车偏左，position = 3.0
     *     L2=1, L1=1 (黑线在左侧)
     *     = (3×1 + 4×1) / 2 = 3.5
     */
    distance = (L4 * 1 + L3 * 2 + L2 * 3 + L1 * 4 +
                R1 * 5 + R2 * 6 + R3 * 7 + R4 * 8) / number;

    /*-------- 异常处理：保留上次有效值 --------*/
    /**
     * 当所有传感器都未检测到黑线时：
     * - number=1，distance=0
     * - 此时使用上次的有效值，避免控制突变
     */
    if (distance != 0)
    {
        /* 当前值有效，保存为上次值 */
        last_distancce = distance;
    }
    else
    {
        /* 当前值无效，使用上次的有效值 */
        distance = last_distancce;
    }
}
