################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User/main.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User/stm32fxxx_it.c 

OBJS += \
./User/main.o \
./User/stm32fxxx_it.o 

C_DEPS += \
./User/main.d \
./User/stm32fxxx_it.d 


# Each subdirectory must supply rules for building sources it contributes
User/main.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F7_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"User/main.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

User/stm32fxxx_it.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User/stm32fxxx_it.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F7_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS_GCC/../00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"User/stm32fxxx_it.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


