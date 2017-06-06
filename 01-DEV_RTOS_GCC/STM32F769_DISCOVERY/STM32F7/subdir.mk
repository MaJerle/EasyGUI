################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User/system_stm32f7xx.c 

OBJS += \
./STM32F7/system_stm32f7xx.o 

C_DEPS += \
./STM32F7/system_stm32f7xx.d 


# Each subdirectory must supply rules for building sources it contributes
STM32F7/system_stm32f7xx.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User/system_stm32f7xx.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"STM32F7/system_stm32f7xx.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


