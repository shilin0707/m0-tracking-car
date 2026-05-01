/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     80000000
/* Defines for SYSPLL_ERR_01 Workaround */
/* Represent 1.000 as 1000 */
#define FLOAT_TO_INT_SCALE                                               (1000U)
#define FCC_EXPECTED_RATIO                                                  2500
#define FCC_UPPER_BOUND                       (FCC_EXPECTED_RATIO * (1 + 0.003))
#define FCC_LOWER_BOUND                       (FCC_EXPECTED_RATIO * (1 - 0.003))

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);


/* Defines for PWM_MOTOR */
#define PWM_MOTOR_INST                                                     TIMG7
#define PWM_MOTOR_INST_IRQHandler                               TIMG7_IRQHandler
#define PWM_MOTOR_INST_INT_IRQN                                 (TIMG7_INT_IRQn)
#define PWM_MOTOR_INST_CLK_FREQ                                          4000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_MOTOR_C0_PORT                                             GPIOA
#define GPIO_PWM_MOTOR_C0_PIN                                      DL_GPIO_PIN_3
#define GPIO_PWM_MOTOR_C0_IOMUX                                   (IOMUX_PINCM8)
#define GPIO_PWM_MOTOR_C0_IOMUX_FUNC                  IOMUX_PINCM8_PF_TIMG7_CCP0
#define GPIO_PWM_MOTOR_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_MOTOR_C1_PORT                                             GPIOA
#define GPIO_PWM_MOTOR_C1_PIN                                      DL_GPIO_PIN_4
#define GPIO_PWM_MOTOR_C1_IOMUX                                   (IOMUX_PINCM9)
#define GPIO_PWM_MOTOR_C1_IOMUX_FUNC                  IOMUX_PINCM9_PF_TIMG7_CCP1
#define GPIO_PWM_MOTOR_C1_IDX                                DL_TIMER_CC_1_INDEX



/* Defines for TIMER_Motor */
#define TIMER_Motor_INST                                                 (TIMG0)
#define TIMER_Motor_INST_IRQHandler                             TIMG0_IRQHandler
#define TIMER_Motor_INST_INT_IRQN                               (TIMG0_INT_IRQn)
#define TIMER_Motor_INST_LOAD_VALUE                                       (999U)
/* Defines for TIMER_Key */
#define TIMER_Key_INST                                                   (TIMG8)
#define TIMER_Key_INST_IRQHandler                               TIMG8_IRQHandler
#define TIMER_Key_INST_INT_IRQN                                 (TIMG8_INT_IRQn)
#define TIMER_Key_INST_LOAD_VALUE                                        (9999U)
/* Defines for TIMER_Grayscale_Sensor */
#define TIMER_Grayscale_Sensor_INST                                      (TIMG6)
#define TIMER_Grayscale_Sensor_INST_IRQHandler                        TIMG6_IRQHandler
#define TIMER_Grayscale_Sensor_INST_INT_IRQN                        (TIMG6_INT_IRQn)
#define TIMER_Grayscale_Sensor_INST_LOAD_VALUE                                 (9999U)
/* Defines for TIMER_Speed */
#define TIMER_Speed_INST                                                (TIMG12)
#define TIMER_Speed_INST_IRQHandler                            TIMG12_IRQHandler
#define TIMER_Speed_INST_INT_IRQN                              (TIMG12_INT_IRQn)
#define TIMER_Speed_INST_LOAD_VALUE                                    (799999U)



/* Defines for UART_BlueSerial */
#define UART_BlueSerial_INST                                               UART3
#define UART_BlueSerial_INST_FREQUENCY                                  80000000
#define UART_BlueSerial_INST_IRQHandler                         UART3_IRQHandler
#define UART_BlueSerial_INST_INT_IRQN                             UART3_INT_IRQn
#define GPIO_UART_BlueSerial_RX_PORT                                       GPIOB
#define GPIO_UART_BlueSerial_TX_PORT                                       GPIOB
#define GPIO_UART_BlueSerial_RX_PIN                                DL_GPIO_PIN_3
#define GPIO_UART_BlueSerial_TX_PIN                                DL_GPIO_PIN_2
#define GPIO_UART_BlueSerial_IOMUX_RX                            (IOMUX_PINCM16)
#define GPIO_UART_BlueSerial_IOMUX_TX                            (IOMUX_PINCM15)
#define GPIO_UART_BlueSerial_IOMUX_RX_FUNC               IOMUX_PINCM16_PF_UART3_RX
#define GPIO_UART_BlueSerial_IOMUX_TX_FUNC               IOMUX_PINCM15_PF_UART3_TX
#define UART_BlueSerial_BAUD_RATE                                         (9600)
#define UART_BlueSerial_IBRD_80_MHZ_9600_BAUD                              (520)
#define UART_BlueSerial_FBRD_80_MHZ_9600_BAUD                               (53)
/* Defines for UART_WIT */
#define UART_WIT_INST                                                      UART2
#define UART_WIT_INST_FREQUENCY                                         40000000
#define UART_WIT_INST_IRQHandler                                UART2_IRQHandler
#define UART_WIT_INST_INT_IRQN                                    UART2_INT_IRQn
#define GPIO_UART_WIT_RX_PORT                                              GPIOA
#define GPIO_UART_WIT_RX_PIN                                      DL_GPIO_PIN_22
#define GPIO_UART_WIT_IOMUX_RX                                   (IOMUX_PINCM47)
#define GPIO_UART_WIT_IOMUX_RX_FUNC                    IOMUX_PINCM47_PF_UART2_RX
#define UART_WIT_BAUD_RATE                                              (115200)
#define UART_WIT_IBRD_40_MHZ_115200_BAUD                                    (21)
#define UART_WIT_FBRD_40_MHZ_115200_BAUD                                    (45)





/* Defines for DMA_WIT */
#define DMA_WIT_CHAN_ID                                                      (0)
#define UART_WIT_INST_DMA_TRIGGER                            (DMA_UART2_RX_TRIG)


/* Port definition for Pin Group GPIO_LED */
#define GPIO_LED_PORT                                                    (GPIOA)

/* Defines for PIN_LED: GPIOA.0 with pinCMx 1 on package pin 33 */
#define GPIO_LED_PIN_LED_PIN                                     (DL_GPIO_PIN_0)
#define GPIO_LED_PIN_LED_IOMUX                                    (IOMUX_PINCM1)
/* Port definition for Pin Group GPIO_Buzzer */
#define GPIO_Buzzer_PORT                                                 (GPIOA)

/* Defines for PIN_Buzzer: GPIOA.27 with pinCMx 60 on package pin 31 */
#define GPIO_Buzzer_PIN_Buzzer_PIN                              (DL_GPIO_PIN_27)
#define GPIO_Buzzer_PIN_Buzzer_IOMUX                             (IOMUX_PINCM60)
/* Defines for PIN_OLED_SCL: GPIOA.17 with pinCMx 39 on package pin 10 */
#define GPIO_OLED_PIN_OLED_SCL_PORT                                      (GPIOA)
#define GPIO_OLED_PIN_OLED_SCL_PIN                              (DL_GPIO_PIN_17)
#define GPIO_OLED_PIN_OLED_SCL_IOMUX                             (IOMUX_PINCM39)
/* Defines for PIN_OLED_SDA: GPIOB.15 with pinCMx 32 on package pin 3 */
#define GPIO_OLED_PIN_OLED_SDA_PORT                                      (GPIOB)
#define GPIO_OLED_PIN_OLED_SDA_PIN                              (DL_GPIO_PIN_15)
#define GPIO_OLED_PIN_OLED_SDA_IOMUX                             (IOMUX_PINCM32)
/* Port definition for Pin Group GPIO_Motor */
#define GPIO_Motor_PORT                                                  (GPIOB)

/* Defines for A_IN1: GPIOB.12 with pinCMx 29 on package pin 64 */
#define GPIO_Motor_A_IN1_PIN                                    (DL_GPIO_PIN_12)
#define GPIO_Motor_A_IN1_IOMUX                                   (IOMUX_PINCM29)
/* Defines for A_IN2: GPIOB.9 with pinCMx 26 on package pin 61 */
#define GPIO_Motor_A_IN2_PIN                                     (DL_GPIO_PIN_9)
#define GPIO_Motor_A_IN2_IOMUX                                   (IOMUX_PINCM26)
/* Defines for B_IN1: GPIOB.10 with pinCMx 27 on package pin 62 */
#define GPIO_Motor_B_IN1_PIN                                    (DL_GPIO_PIN_10)
#define GPIO_Motor_B_IN1_IOMUX                                   (IOMUX_PINCM27)
/* Defines for B_IN2: GPIOB.11 with pinCMx 28 on package pin 63 */
#define GPIO_Motor_B_IN2_PIN                                    (DL_GPIO_PIN_11)
#define GPIO_Motor_B_IN2_IOMUX                                   (IOMUX_PINCM28)
/* Port definition for Pin Group GPIO_Encoder */
#define GPIO_Encoder_PORT                                                (GPIOB)

/* Defines for Left_A: GPIOB.7 with pinCMx 24 on package pin 59 */
// pins affected by this interrupt request:["Left_A","Left_B","Right_A","Right_B"]
#define GPIO_Encoder_INT_IRQN                                   (GPIOB_INT_IRQn)
#define GPIO_Encoder_INT_IIDX                   (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define GPIO_Encoder_Left_A_IIDX                             (DL_GPIO_IIDX_DIO7)
#define GPIO_Encoder_Left_A_PIN                                  (DL_GPIO_PIN_7)
#define GPIO_Encoder_Left_A_IOMUX                                (IOMUX_PINCM24)
/* Defines for Left_B: GPIOB.6 with pinCMx 23 on package pin 58 */
#define GPIO_Encoder_Left_B_IIDX                             (DL_GPIO_IIDX_DIO6)
#define GPIO_Encoder_Left_B_PIN                                  (DL_GPIO_PIN_6)
#define GPIO_Encoder_Left_B_IOMUX                                (IOMUX_PINCM23)
/* Defines for Right_A: GPIOB.5 with pinCMx 18 on package pin 53 */
#define GPIO_Encoder_Right_A_IIDX                            (DL_GPIO_IIDX_DIO5)
#define GPIO_Encoder_Right_A_PIN                                 (DL_GPIO_PIN_5)
#define GPIO_Encoder_Right_A_IOMUX                               (IOMUX_PINCM18)
/* Defines for Right_B: GPIOB.4 with pinCMx 17 on package pin 52 */
#define GPIO_Encoder_Right_B_IIDX                            (DL_GPIO_IIDX_DIO4)
#define GPIO_Encoder_Right_B_PIN                                 (DL_GPIO_PIN_4)
#define GPIO_Encoder_Right_B_IOMUX                               (IOMUX_PINCM17)
/* Defines for PIN_L_4: GPIOA.1 with pinCMx 2 on package pin 34 */
#define GPIO_Grayscale_Sensor_PIN_L_4_PORT                               (GPIOA)
#define GPIO_Grayscale_Sensor_PIN_L_4_PIN                        (DL_GPIO_PIN_1)
#define GPIO_Grayscale_Sensor_PIN_L_4_IOMUX                       (IOMUX_PINCM2)
/* Defines for PIN_L_3: GPIOA.28 with pinCMx 3 on package pin 35 */
#define GPIO_Grayscale_Sensor_PIN_L_3_PORT                               (GPIOA)
#define GPIO_Grayscale_Sensor_PIN_L_3_PIN                       (DL_GPIO_PIN_28)
#define GPIO_Grayscale_Sensor_PIN_L_3_IOMUX                       (IOMUX_PINCM3)
/* Defines for PIN_L_2: GPIOA.31 with pinCMx 6 on package pin 39 */
#define GPIO_Grayscale_Sensor_PIN_L_2_PORT                               (GPIOA)
#define GPIO_Grayscale_Sensor_PIN_L_2_PIN                       (DL_GPIO_PIN_31)
#define GPIO_Grayscale_Sensor_PIN_L_2_IOMUX                       (IOMUX_PINCM6)
/* Defines for PIN_L_1: GPIOB.13 with pinCMx 30 on package pin 1 */
#define GPIO_Grayscale_Sensor_PIN_L_1_PORT                               (GPIOB)
#define GPIO_Grayscale_Sensor_PIN_L_1_PIN                       (DL_GPIO_PIN_13)
#define GPIO_Grayscale_Sensor_PIN_L_1_IOMUX                      (IOMUX_PINCM30)
/* Defines for PIN_R_1: GPIOA.16 with pinCMx 38 on package pin 9 */
#define GPIO_Grayscale_Sensor_PIN_R_1_PORT                               (GPIOA)
#define GPIO_Grayscale_Sensor_PIN_R_1_PIN                       (DL_GPIO_PIN_16)
#define GPIO_Grayscale_Sensor_PIN_R_1_IOMUX                      (IOMUX_PINCM38)
/* Defines for PIN_R_2: GPIOB.19 with pinCMx 45 on package pin 16 */
#define GPIO_Grayscale_Sensor_PIN_R_2_PORT                               (GPIOB)
#define GPIO_Grayscale_Sensor_PIN_R_2_PIN                       (DL_GPIO_PIN_19)
#define GPIO_Grayscale_Sensor_PIN_R_2_IOMUX                      (IOMUX_PINCM45)
/* Defines for PIN_R_3: GPIOA.24 with pinCMx 54 on package pin 25 */
#define GPIO_Grayscale_Sensor_PIN_R_3_PORT                               (GPIOA)
#define GPIO_Grayscale_Sensor_PIN_R_3_PIN                       (DL_GPIO_PIN_24)
#define GPIO_Grayscale_Sensor_PIN_R_3_IOMUX                      (IOMUX_PINCM54)
/* Defines for PIN_R_4: GPIOA.25 with pinCMx 55 on package pin 26 */
#define GPIO_Grayscale_Sensor_PIN_R_4_PORT                               (GPIOA)
#define GPIO_Grayscale_Sensor_PIN_R_4_PIN                       (DL_GPIO_PIN_25)
#define GPIO_Grayscale_Sensor_PIN_R_4_IOMUX                      (IOMUX_PINCM55)
/* Port definition for Pin Group GPIO_Key */
#define GPIO_Key_PORT                                                    (GPIOB)

/* Defines for PIN_Key_0: GPIOB.18 with pinCMx 44 on package pin 15 */
#define GPIO_Key_PIN_Key_0_PIN                                  (DL_GPIO_PIN_18)
#define GPIO_Key_PIN_Key_0_IOMUX                                 (IOMUX_PINCM44)
/* Defines for PIN_Key_1: GPIOB.21 with pinCMx 49 on package pin 20 */
#define GPIO_Key_PIN_Key_1_PIN                                  (DL_GPIO_PIN_21)
#define GPIO_Key_PIN_Key_1_IOMUX                                 (IOMUX_PINCM49)
/* Defines for PIN_Key_2: GPIOB.23 with pinCMx 51 on package pin 22 */
#define GPIO_Key_PIN_Key_2_PIN                                  (DL_GPIO_PIN_23)
#define GPIO_Key_PIN_Key_2_IOMUX                                 (IOMUX_PINCM51)
/* Defines for PIN_Key_3: GPIOB.24 with pinCMx 52 on package pin 23 */
#define GPIO_Key_PIN_Key_3_PIN                                  (DL_GPIO_PIN_24)
#define GPIO_Key_PIN_Key_3_IOMUX                                 (IOMUX_PINCM52)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);
void SYSCFG_DL_PWM_MOTOR_init(void);
void SYSCFG_DL_TIMER_Motor_init(void);
void SYSCFG_DL_TIMER_Key_init(void);
void SYSCFG_DL_TIMER_Grayscale_Sensor_init(void);
void SYSCFG_DL_TIMER_Speed_init(void);
void SYSCFG_DL_UART_BlueSerial_init(void);
void SYSCFG_DL_UART_WIT_init(void);
void SYSCFG_DL_DMA_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
