################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/gui.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/gui_draw.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/gui_ll.c 

OBJS += \
./GUI/gui.o \
./GUI/gui_draw.o \
./GUI/gui_ll.o 

C_DEPS += \
./GUI/gui.d \
./GUI/gui_draw.d \
./GUI/gui_ll.d 


# Each subdirectory must supply rules for building sources it contributes
GUI/gui.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/gui.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/gui.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/gui_draw.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/gui_draw.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/gui_draw.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/gui_ll.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/gui_ll.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/gui_ll.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


