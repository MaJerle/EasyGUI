################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f746xx.s 

OBJS += \
./STM32F7/Startup/startup_stm32f746xx.o 


# Each subdirectory must supply rules for building sources it contributes
STM32F7/Startup/startup_stm32f746xx.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f746xx.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo %cd%
	arm-none-eabi-as -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES/FreeRTOS" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES/FreeRTOS/Include" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES/FreeRTOS/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES/FreeRTOS/CMSIS_RTOS" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


