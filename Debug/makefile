################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS

GEN_OPTS__FLAG := @"./device.opt" 
GEN_CMDS__FLAG := -Wl,-l"./device_linker.cmd" 

ORDERED_OBJS += \
"./main.o" \
"./ti_msp_dl_config.o" \
"./startup_mspm0g350x_ticlang.o" \
"./Drive/BlueSerial/BlueSerial.o" \
"./Drive/Buzzer/Buzzer.o" \
"./Drive/Encoder/Encoder.o" \
"./Drive/Key/Key.o" \
"./Drive/MSPM0/clock.o" \
"./Drive/MSPM0/interrupt.o" \
"./Drive/Menu/Menu.o" \
"./Drive/Motor/motor.o" \
"./Drive/OLED_Software_I2C/oled_software_i2c.o" \
"./Drive/OLED_Software_I2C/oledfont.o" \
"./Drive/PID/PID.o" \
"./Drive/PWM/pwm.o" \
"./Drive/WIT/wit.o" \
"./Drive/grayscale_sensor/grayscale_sensor.o" \
$(GEN_CMDS__FLAG) \
-Wl,-ldevice.cmd.genlibs \
-Wl,-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Drive/BlueSerial/subdir_vars.mk
-include Drive/Buzzer/subdir_vars.mk
-include Drive/Encoder/subdir_vars.mk
-include Drive/Key/subdir_vars.mk
-include Drive/MSPM0/subdir_vars.mk
-include Drive/Menu/subdir_vars.mk
-include Drive/Motor/subdir_vars.mk
-include Drive/OLED_Software_I2C/subdir_vars.mk
-include Drive/PID/subdir_vars.mk
-include Drive/PWM/subdir_vars.mk
-include Drive/WIT/subdir_vars.mk
-include Drive/grayscale_sensor/subdir_vars.mk
-include subdir_rules.mk
-include Drive/BlueSerial/subdir_rules.mk
-include Drive/Buzzer/subdir_rules.mk
-include Drive/Encoder/subdir_rules.mk
-include Drive/Key/subdir_rules.mk
-include Drive/MSPM0/subdir_rules.mk
-include Drive/Menu/subdir_rules.mk
-include Drive/Motor/subdir_rules.mk
-include Drive/OLED_Software_I2C/subdir_rules.mk
-include Drive/PID/subdir_rules.mk
-include Drive/PWM/subdir_rules.mk
-include Drive/WIT/subdir_rules.mk
-include Drive/grayscale_sensor/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
m0\ tacking\ car.out 

EXE_OUTPUTS__QUOTED += \
"m0 tacking car.out" 

BIN_OUTPUTS += \
m0\ tacking\ car.hex 

BIN_OUTPUTS__QUOTED += \
"m0 tacking car.hex" 


# All Target
all: $(OBJS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "m0 tacking car.out" secondary-outputs

# Tool invocations
m0\ tacking\ car.out: $(OBJS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -gdwarf-3 -Wall -Wl,-m"m0 tacking car.map" -Wl,-i"C:/TI/mspm0_sdk_2_10_00_04/source" -Wl,-i"C:/Users/zhao/workspace_ccstheia/m0 tacking car" -Wl,-i"C:/Users/zhao/workspace_ccstheia/m0 tacking car/Debug/syscfg" -Wl,-i"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/lib" -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections -Wl,--xml_link_info="m0 tacking car_linkInfo.xml" -Wl,--rom_model -o "m0 tacking car.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

m0\ tacking\ car.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: Arm Hex Utility'
	"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmhex.exe" --byte --memwidth=8 --romwidth=8 --diag_wrap=off --intel -o "m0 tacking car.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.o" "ti_msp_dl_config.o" "startup_mspm0g350x_ticlang.o" "Drive\BlueSerial\BlueSerial.o" "Drive\Buzzer\Buzzer.o" "Drive\Encoder\Encoder.o" "Drive\Key\Key.o" "Drive\MSPM0\clock.o" "Drive\MSPM0\interrupt.o" "Drive\Menu\Menu.o" "Drive\Motor\motor.o" "Drive\OLED_Software_I2C\oled_software_i2c.o" "Drive\OLED_Software_I2C\oledfont.o" "Drive\PID\PID.o" "Drive\PWM\pwm.o" "Drive\WIT\wit.o" "Drive\grayscale_sensor\grayscale_sensor.o" 
	-$(RM) "main.d" "ti_msp_dl_config.d" "startup_mspm0g350x_ticlang.d" "Drive\BlueSerial\BlueSerial.d" "Drive\Buzzer\Buzzer.d" "Drive\Encoder\Encoder.d" "Drive\Key\Key.d" "Drive\MSPM0\clock.d" "Drive\MSPM0\interrupt.d" "Drive\Menu\Menu.d" "Drive\Motor\motor.d" "Drive\OLED_Software_I2C\oled_software_i2c.d" "Drive\OLED_Software_I2C\oledfont.d" "Drive\PID\PID.d" "Drive\PWM\pwm.d" "Drive\WIT\wit.d" "Drive\grayscale_sensor\grayscale_sensor.d" 
	-@echo 'Finished clean'
	-@echo ' '

secondary-outputs: $(BIN_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

