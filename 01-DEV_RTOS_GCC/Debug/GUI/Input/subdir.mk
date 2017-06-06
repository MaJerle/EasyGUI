################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/input/gui_input.c 

OBJS += \
./GUI/Input/gui_input.o 

C_DEPS += \
./GUI/Input/gui_input.d 


# Each subdirectory must supply rules for building sources it contributes
GUI/Input/gui_input.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/input/gui_input.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Input/gui_input.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


