################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_buffer.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_delay.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_disco.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_exti.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_general.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_gpio.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_i2c.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_rcc.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_sdram.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_touch.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_touch_ft5336.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_touch_ft6206.c \
C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_usart.c 

OBJS += \
./TM/tm_stm32_buffer.o \
./TM/tm_stm32_delay.o \
./TM/tm_stm32_disco.o \
./TM/tm_stm32_exti.o \
./TM/tm_stm32_general.o \
./TM/tm_stm32_gpio.o \
./TM/tm_stm32_i2c.o \
./TM/tm_stm32_rcc.o \
./TM/tm_stm32_sdram.o \
./TM/tm_stm32_touch.o \
./TM/tm_stm32_touch_ft5336.o \
./TM/tm_stm32_touch_ft6206.o \
./TM/tm_stm32_usart.o 

C_DEPS += \
./TM/tm_stm32_buffer.d \
./TM/tm_stm32_delay.d \
./TM/tm_stm32_disco.d \
./TM/tm_stm32_exti.d \
./TM/tm_stm32_general.d \
./TM/tm_stm32_gpio.d \
./TM/tm_stm32_i2c.d \
./TM/tm_stm32_rcc.d \
./TM/tm_stm32_sdram.d \
./TM/tm_stm32_touch.d \
./TM/tm_stm32_touch_ft5336.d \
./TM/tm_stm32_touch_ft6206.d \
./TM/tm_stm32_usart.d 


# Each subdirectory must supply rules for building sources it contributes
TM/tm_stm32_buffer.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_buffer.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_buffer.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_delay.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_delay.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_delay.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_disco.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_disco.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_disco.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_exti.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_exti.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_exti.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_general.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_general.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_general.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_gpio.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_gpio.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_i2c.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_i2c.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_rcc.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_rcc.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_rcc.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_sdram.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_sdram.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_sdram.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_touch.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_touch.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_touch.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_touch_ft5336.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_touch_ft5336.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_touch_ft5336.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_touch_ft6206.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_touch_ft6206.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_touch_ft6206.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TM/tm_stm32_usart.o: C:/Users/MajerleT/Google\ Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES/tm_stm32_usart.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F769xx -DSTM32F7xx '-DHSE_VALUE=25000000' -DSTM32F769_DISCOVERY '-DRCC_OSCILLATORTYPE=RCC_OSCILLATORTYPE_HSE' '-DRCC_PLLM=25' '-DRCC_PLLN=400' '-DRCC_PLLP=2' '-DRCC_PLLQ=7' '-DRCC_PLLR=6' '-DRCC_PLLSOURCE=RCC_PLLSOURCE_HSE' -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/01-DEV_RTOS/User" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/input" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/widgets" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/pt" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-GUI_LIBRARY/utils" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-STM32_LIBRARIES" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/MajerleT/Google Drive/ELECTRONICS/ARM/gui/00-HAL_DRIVERS/CMSIS/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"TM/tm_stm32_usart.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


