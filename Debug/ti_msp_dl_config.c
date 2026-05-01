/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerG_backupConfig gPWM_MOTORBackup;
DL_TimerG_backupConfig gTIMER_Grayscale_SensorBackup;
DL_UART_Main_backupConfig gUART_BlueSerialBackup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_PWM_MOTOR_init();
    SYSCFG_DL_TIMER_Motor_init();
    SYSCFG_DL_TIMER_Key_init();
    SYSCFG_DL_TIMER_Grayscale_Sensor_init();
    SYSCFG_DL_TIMER_Speed_init();
    SYSCFG_DL_UART_BlueSerial_init();
    SYSCFG_DL_UART_WIT_init();
    SYSCFG_DL_DMA_init();
    SYSCFG_DL_SYSTICK_init();
    /* Ensure backup structures have no valid state */
	gPWM_MOTORBackup.backupRdy 	= false;
	gTIMER_Grayscale_SensorBackup.backupRdy 	= false;
	gUART_BlueSerialBackup.backupRdy 	= false;

}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerG_saveConfiguration(PWM_MOTOR_INST, &gPWM_MOTORBackup);
	retStatus &= DL_TimerG_saveConfiguration(TIMER_Grayscale_Sensor_INST, &gTIMER_Grayscale_SensorBackup);
	retStatus &= DL_UART_Main_saveConfiguration(UART_BlueSerial_INST, &gUART_BlueSerialBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerG_restoreConfiguration(PWM_MOTOR_INST, &gPWM_MOTORBackup, false);
	retStatus &= DL_TimerG_restoreConfiguration(TIMER_Grayscale_Sensor_INST, &gTIMER_Grayscale_SensorBackup, false);
	retStatus &= DL_UART_Main_restoreConfiguration(UART_BlueSerial_INST, &gUART_BlueSerialBackup);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerG_reset(PWM_MOTOR_INST);
    DL_TimerG_reset(TIMER_Motor_INST);
    DL_TimerG_reset(TIMER_Key_INST);
    DL_TimerG_reset(TIMER_Grayscale_Sensor_INST);
    DL_TimerG_reset(TIMER_Speed_INST);
    DL_UART_Main_reset(UART_BlueSerial_INST);
    DL_UART_Main_reset(UART_WIT_INST);



    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerG_enablePower(PWM_MOTOR_INST);
    DL_TimerG_enablePower(TIMER_Motor_INST);
    DL_TimerG_enablePower(TIMER_Key_INST);
    DL_TimerG_enablePower(TIMER_Grayscale_Sensor_INST);
    DL_TimerG_enablePower(TIMER_Speed_INST);
    DL_UART_Main_enablePower(UART_BlueSerial_INST);
    DL_UART_Main_enablePower(UART_WIT_INST);


    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_MOTOR_C0_IOMUX,GPIO_PWM_MOTOR_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_MOTOR_C0_PORT, GPIO_PWM_MOTOR_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_MOTOR_C1_IOMUX,GPIO_PWM_MOTOR_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_MOTOR_C1_PORT, GPIO_PWM_MOTOR_C1_PIN);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_BlueSerial_IOMUX_TX, GPIO_UART_BlueSerial_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_BlueSerial_IOMUX_RX, GPIO_UART_BlueSerial_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_WIT_IOMUX_RX, GPIO_UART_WIT_IOMUX_RX_FUNC);

    DL_GPIO_initDigitalOutputFeatures(GPIO_LED_PIN_LED_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_DRIVE_STRENGTH_LOW, DL_GPIO_HIZ_DISABLE);

    DL_GPIO_initDigitalOutputFeatures(GPIO_Buzzer_PIN_Buzzer_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_DRIVE_STRENGTH_LOW, DL_GPIO_HIZ_DISABLE);

    DL_GPIO_initDigitalOutput(GPIO_OLED_PIN_OLED_SCL_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_OLED_PIN_OLED_SDA_IOMUX);

    DL_GPIO_initDigitalOutputFeatures(GPIO_Motor_A_IN1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_DRIVE_STRENGTH_LOW, DL_GPIO_HIZ_DISABLE);

    DL_GPIO_initDigitalOutputFeatures(GPIO_Motor_A_IN2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_DRIVE_STRENGTH_LOW, DL_GPIO_HIZ_DISABLE);

    DL_GPIO_initDigitalOutputFeatures(GPIO_Motor_B_IN1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_DRIVE_STRENGTH_LOW, DL_GPIO_HIZ_DISABLE);

    DL_GPIO_initDigitalOutputFeatures(GPIO_Motor_B_IN2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_DRIVE_STRENGTH_LOW, DL_GPIO_HIZ_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Encoder_Left_A_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Encoder_Left_B_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Encoder_Right_A_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_DOWN,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Encoder_Right_B_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInput(GPIO_Grayscale_Sensor_PIN_L_4_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_Grayscale_Sensor_PIN_L_3_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Grayscale_Sensor_PIN_L_2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Grayscale_Sensor_PIN_L_1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Grayscale_Sensor_PIN_R_1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Grayscale_Sensor_PIN_R_2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Grayscale_Sensor_PIN_R_3_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Grayscale_Sensor_PIN_R_4_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Key_PIN_Key_0_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Key_PIN_Key_1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Key_PIN_Key_2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Key_PIN_Key_3_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_clearPins(GPIOA, GPIO_Buzzer_PIN_Buzzer_PIN |
		GPIO_OLED_PIN_OLED_SCL_PIN);
    DL_GPIO_setPins(GPIOA, GPIO_LED_PIN_LED_PIN);
    DL_GPIO_enableOutput(GPIOA, GPIO_LED_PIN_LED_PIN |
		GPIO_Buzzer_PIN_Buzzer_PIN |
		GPIO_OLED_PIN_OLED_SCL_PIN);
    DL_GPIO_clearPins(GPIOB, GPIO_OLED_PIN_OLED_SDA_PIN |
		GPIO_Motor_A_IN1_PIN |
		GPIO_Motor_A_IN2_PIN |
		GPIO_Motor_B_IN1_PIN |
		GPIO_Motor_B_IN2_PIN);
    DL_GPIO_enableOutput(GPIOB, GPIO_OLED_PIN_OLED_SDA_PIN |
		GPIO_Motor_A_IN1_PIN |
		GPIO_Motor_A_IN2_PIN |
		GPIO_Motor_B_IN1_PIN |
		GPIO_Motor_B_IN2_PIN);
    DL_GPIO_setLowerPinsPolarity(GPIOB, DL_GPIO_PIN_7_EDGE_RISE |
		DL_GPIO_PIN_6_EDGE_FALL |
		DL_GPIO_PIN_5_EDGE_RISE |
		DL_GPIO_PIN_4_EDGE_FALL);
    DL_GPIO_clearInterruptStatus(GPIOB, GPIO_Encoder_Left_A_PIN |
		GPIO_Encoder_Left_B_PIN |
		GPIO_Encoder_Right_A_PIN |
		GPIO_Encoder_Right_B_PIN);
    DL_GPIO_enableInterrupt(GPIOB, GPIO_Encoder_Left_A_PIN |
		GPIO_Encoder_Left_B_PIN |
		GPIO_Encoder_Right_A_PIN |
		GPIO_Encoder_Right_B_PIN);

}


static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {
    .inputFreq              = DL_SYSCTL_SYSPLL_INPUT_FREQ_16_32_MHZ,
	.rDivClk2x              = 1,
	.rDivClk1               = 0,
	.rDivClk0               = 0,
	.enableCLK2x            = DL_SYSCTL_SYSPLL_CLK2X_ENABLE,
	.enableCLK1             = DL_SYSCTL_SYSPLL_CLK1_DISABLE,
	.enableCLK0             = DL_SYSCTL_SYSPLL_CLK0_DISABLE,
	.sysPLLMCLK             = DL_SYSCTL_SYSPLL_MCLK_CLK2X,
	.sysPLLRef              = DL_SYSCTL_SYSPLL_REF_SYSOSC,
	.qDiv                   = 4,
	.pDiv                   = DL_SYSCTL_SYSPLL_PDIV_2
};

SYSCONFIG_WEAK bool SYSCFG_DL_SYSCTL_SYSPLL_init(void)
{
    bool fFCCRatioStatus = false;
    uint32_t fFCCSysoscCount;
    uint32_t fFCCPllCount;
    uint32_t fFCCRatio;
    uint32_t fccTimeOutCounter;

    DL_SYSCTL_setFCCPeriods( DL_SYSCTL_FCC_TRIG_CNT_01 );

    /* Measuring PLL. */
    DL_SYSCTL_configFCC(DL_SYSCTL_FCC_TRIG_TYPE_RISE_RISE,
                        DL_SYSCTL_FCC_TRIG_SOURCE_LFCLK,
                        DL_SYSCTL_FCC_CLOCK_SOURCE_SYSPLLCLK2X);
    /* Get SYSPLL frequency using FCC */
    fccTimeOutCounter = 0;
    DL_SYSCTL_startFCC();
    while (DL_SYSCTL_isFCCDone() == 0) {
        delay_cycles(977);  /* 1x LFCLK cycle = 32MHz/32.768kHz = 977, 30.5us */
        fccTimeOutCounter++;
        if(fccTimeOutCounter > 65){
            /* Timeout set to approximately 2ms (user-customizable) */
            break;
        }
    }

    /* get measA= SYSPLLCLK2X freq wrt LFOSC*/
    fFCCPllCount = DL_SYSCTL_readFCC();

    /* Measuring SYSPLL Source */
    DL_SYSCTL_configFCC(DL_SYSCTL_FCC_TRIG_TYPE_RISE_RISE,
                        DL_SYSCTL_FCC_TRIG_SOURCE_LFCLK,
                        DL_SYSCTL_FCC_CLOCK_SOURCE_SYSOSC);
    /* Get SYSPLL frequency using FCC */
    fccTimeOutCounter = 0;
    DL_SYSCTL_startFCC();
    while (DL_SYSCTL_isFCCDone() == 0) {
        delay_cycles(977);  /* 1x LFCLK cycle = 32MHz/32.768kHz = 977, 30.5us */
        fccTimeOutCounter++;
        if(fccTimeOutCounter > 65){
            /* Timeout set to approximately 2ms (user-customizable) */
            break;
        }
    }

    /* get measB= SYSOSC freq wrt LFOSC*/
    fFCCSysoscCount = DL_SYSCTL_readFCC();

    /* Get ratio of both measurements*/
    fFCCRatio = (fFCCPllCount * FLOAT_TO_INT_SCALE) / fFCCSysoscCount;
    /* Check ratio is within bounds*/
    if ((FCC_LOWER_BOUND <  fFCCRatio) && (fFCCRatio < FCC_UPPER_BOUND))
    {
        /* ratio is good for proceeding into application code. */
        fFCCRatioStatus = true;
    }

    return fFCCRatioStatus;
}
SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setFlashWaitState(DL_SYSCTL_FLASH_WAIT_STATE_2);

    
	DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
	/* Set default configuration */
	DL_SYSCTL_disableHFXT();
	DL_SYSCTL_disableSYSPLL();
    DL_SYSCTL_configSYSPLL((DL_SYSCTL_SYSPLLConfig *) &gSYSPLLConfig);

    /*
     * [SYSPLL_ERR_01]
     * PLL Incorrect locking WA start.
     * Insert after every PLL enable.
     * This can lead an infinite loop if the condition persists
     * and can block entry to the application code.
     */

    while (SYSCFG_DL_SYSCTL_SYSPLL_init() == false)
    {
        /* Toggle SYSPLL enable to re-enable SYSPLL and re-check incorrect locking */
        DL_SYSCTL_disableSYSPLL();
        DL_SYSCTL_enableSYSPLL();

        /* Wait until SYSPLL startup is stabilized*/
        while ((DL_SYSCTL_getClockStatus() & SYSCTL_CLKSTATUS_SYSPLLGOOD_MASK) != DL_SYSCTL_CLK_STATUS_SYSPLL_GOOD){}
    }
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_2);
    DL_SYSCTL_setMCLKSource(SYSOSC, HSCLK, DL_SYSCTL_HSCLK_SOURCE_SYSPLL);

}


/*
 * Timer clock configuration to be sourced by  / 2 (40000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   4000000 Hz = 40000000 Hz / (2 * (9 + 1))
 */
static const DL_TimerG_ClockConfig gPWM_MOTORClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_2,
    .prescale = 9U
};

static const DL_TimerG_PWMConfig gPWM_MOTORConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
    .period = 4000,
    .isTimerWithFourCC = false,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_MOTOR_init(void) {

    DL_TimerG_setClockConfig(
        PWM_MOTOR_INST, (DL_TimerG_ClockConfig *) &gPWM_MOTORClockConfig);

    DL_TimerG_initPWMMode(
        PWM_MOTOR_INST, (DL_TimerG_PWMConfig *) &gPWM_MOTORConfig);

    // Set Counter control to the smallest CC index being used
    DL_TimerG_setCounterControl(PWM_MOTOR_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerG_setCaptureCompareOutCtl(PWM_MOTOR_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_0_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(PWM_MOTOR_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST, 4000, DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareOutCtl(PWM_MOTOR_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_1_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(PWM_MOTOR_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_MOTOR_INST, 4000, DL_TIMER_CC_1_INDEX);

    DL_TimerG_enableClock(PWM_MOTOR_INST);


    
    DL_TimerG_setCCPDirection(PWM_MOTOR_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );


}



/*
 * Timer clock configuration to be sourced by BUSCLK /  (40000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 40000000 Hz / (1 * (39 + 1))
 */
static const DL_TimerG_ClockConfig gTIMER_MotorClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 39U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_Motor_INST_LOAD_VALUE = (1 ms * 1000000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTIMER_MotorTimerConfig = {
    .period     = TIMER_Motor_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_Motor_init(void) {

    DL_TimerG_setClockConfig(TIMER_Motor_INST,
        (DL_TimerG_ClockConfig *) &gTIMER_MotorClockConfig);

    DL_TimerG_initTimerMode(TIMER_Motor_INST,
        (DL_TimerG_TimerConfig *) &gTIMER_MotorTimerConfig);
    DL_TimerG_enableInterrupt(TIMER_Motor_INST , DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TIMER_Motor_INST);





}

/*
 * Timer clock configuration to be sourced by BUSCLK /  (40000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 40000000 Hz / (1 * (39 + 1))
 */
static const DL_TimerG_ClockConfig gTIMER_KeyClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 39U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_Key_INST_LOAD_VALUE = (10 ms * 1000000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTIMER_KeyTimerConfig = {
    .period     = TIMER_Key_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_Key_init(void) {

    DL_TimerG_setClockConfig(TIMER_Key_INST,
        (DL_TimerG_ClockConfig *) &gTIMER_KeyClockConfig);

    DL_TimerG_initTimerMode(TIMER_Key_INST,
        (DL_TimerG_TimerConfig *) &gTIMER_KeyTimerConfig);
    DL_TimerG_enableInterrupt(TIMER_Key_INST , DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TIMER_Key_INST);





}

/*
 * Timer clock configuration to be sourced by BUSCLK /  (80000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 80000000 Hz / (1 * (79 + 1))
 */
static const DL_TimerG_ClockConfig gTIMER_Grayscale_SensorClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 79U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_Grayscale_Sensor_INST_LOAD_VALUE = (10 ms * 1000000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTIMER_Grayscale_SensorTimerConfig = {
    .period     = TIMER_Grayscale_Sensor_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_Grayscale_Sensor_init(void) {

    DL_TimerG_setClockConfig(TIMER_Grayscale_Sensor_INST,
        (DL_TimerG_ClockConfig *) &gTIMER_Grayscale_SensorClockConfig);

    DL_TimerG_initTimerMode(TIMER_Grayscale_Sensor_INST,
        (DL_TimerG_TimerConfig *) &gTIMER_Grayscale_SensorTimerConfig);
    DL_TimerG_enableInterrupt(TIMER_Grayscale_Sensor_INST , DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TIMER_Grayscale_Sensor_INST);





}

/*
 * Timer clock configuration to be sourced by BUSCLK /  (80000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   80000000 Hz = 80000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gTIMER_SpeedClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 0U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_Speed_INST_LOAD_VALUE = (10 ms * 80000000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTIMER_SpeedTimerConfig = {
    .period     = TIMER_Speed_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_Speed_init(void) {

    DL_TimerG_setClockConfig(TIMER_Speed_INST,
        (DL_TimerG_ClockConfig *) &gTIMER_SpeedClockConfig);

    DL_TimerG_initTimerMode(TIMER_Speed_INST,
        (DL_TimerG_TimerConfig *) &gTIMER_SpeedTimerConfig);
    DL_TimerG_enableInterrupt(TIMER_Speed_INST , DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TIMER_Speed_INST);





}


static const DL_UART_Main_ClockConfig gUART_BlueSerialClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_BlueSerialConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_BlueSerial_init(void)
{
    DL_UART_Main_setClockConfig(UART_BlueSerial_INST, (DL_UART_Main_ClockConfig *) &gUART_BlueSerialClockConfig);

    DL_UART_Main_init(UART_BlueSerial_INST, (DL_UART_Main_Config *) &gUART_BlueSerialConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 9600
     *  Actual baud rate: 9600.1
     */
    DL_UART_Main_setOversampling(UART_BlueSerial_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_BlueSerial_INST, UART_BlueSerial_IBRD_80_MHZ_9600_BAUD, UART_BlueSerial_FBRD_80_MHZ_9600_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_BlueSerial_INST,
                                 DL_UART_MAIN_INTERRUPT_RX);


    DL_UART_Main_enable(UART_BlueSerial_INST);
}
static const DL_UART_Main_ClockConfig gUART_WITClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_WITConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_WIT_init(void)
{
    DL_UART_Main_setClockConfig(UART_WIT_INST, (DL_UART_Main_ClockConfig *) &gUART_WITClockConfig);

    DL_UART_Main_init(UART_WIT_INST, (DL_UART_Main_Config *) &gUART_WITConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART_WIT_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_WIT_INST, UART_WIT_IBRD_40_MHZ_115200_BAUD, UART_WIT_FBRD_40_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_WIT_INST,
                                 DL_UART_MAIN_INTERRUPT_RX_TIMEOUT_ERROR);

    /* Configure DMA Receive Event */
    DL_UART_Main_enableDMAReceiveEvent(UART_WIT_INST, DL_UART_DMA_INTERRUPT_RX);
    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART_WIT_INST);
    DL_UART_Main_setRXFIFOThreshold(UART_WIT_INST, DL_UART_RX_FIFO_LEVEL_1_2_FULL);

    DL_UART_Main_enable(UART_WIT_INST);
}

static const DL_DMA_Config gDMA_WITConfig = {
    .transferMode   = DL_DMA_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_INCREMENT,
    .srcIncrement   = DL_DMA_ADDR_UNCHANGED,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = UART_WIT_INST_DMA_TRIGGER,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_WIT_init(void)
{
    DL_DMA_initChannel(DMA, DMA_WIT_CHAN_ID , (DL_DMA_Config *) &gDMA_WITConfig);
}
SYSCONFIG_WEAK void SYSCFG_DL_DMA_init(void){
    SYSCFG_DL_DMA_WIT_init();
}


SYSCONFIG_WEAK void SYSCFG_DL_SYSTICK_init(void)
{
    /*
     * Initializes the SysTick period to 1.00 ms,
     * enables the interrupt, and starts the SysTick Timer
     */
    DL_SYSTICK_config(80000);
}

