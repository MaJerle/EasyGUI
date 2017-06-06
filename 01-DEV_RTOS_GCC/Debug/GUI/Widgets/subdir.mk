################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_button.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_checkbox.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_dialog.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_dropdown.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_edittext.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_graph.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_image.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_led.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_listbox.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_listview.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_progbar.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_radio.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_slider.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_textview.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_widget.c \
C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_window.c 

OBJS += \
./GUI/Widgets/gui_button.o \
./GUI/Widgets/gui_checkbox.o \
./GUI/Widgets/gui_dialog.o \
./GUI/Widgets/gui_dropdown.o \
./GUI/Widgets/gui_edittext.o \
./GUI/Widgets/gui_graph.o \
./GUI/Widgets/gui_image.o \
./GUI/Widgets/gui_led.o \
./GUI/Widgets/gui_listbox.o \
./GUI/Widgets/gui_listview.o \
./GUI/Widgets/gui_progbar.o \
./GUI/Widgets/gui_radio.o \
./GUI/Widgets/gui_slider.o \
./GUI/Widgets/gui_textview.o \
./GUI/Widgets/gui_widget.o \
./GUI/Widgets/gui_window.o 

C_DEPS += \
./GUI/Widgets/gui_button.d \
./GUI/Widgets/gui_checkbox.d \
./GUI/Widgets/gui_dialog.d \
./GUI/Widgets/gui_dropdown.d \
./GUI/Widgets/gui_edittext.d \
./GUI/Widgets/gui_graph.d \
./GUI/Widgets/gui_image.d \
./GUI/Widgets/gui_led.d \
./GUI/Widgets/gui_listbox.d \
./GUI/Widgets/gui_listview.d \
./GUI/Widgets/gui_progbar.d \
./GUI/Widgets/gui_radio.d \
./GUI/Widgets/gui_slider.d \
./GUI/Widgets/gui_textview.d \
./GUI/Widgets/gui_widget.d \
./GUI/Widgets/gui_window.d 


# Each subdirectory must supply rules for building sources it contributes
GUI/Widgets/gui_button.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_button.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_button.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_checkbox.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_checkbox.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_checkbox.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_dialog.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_dialog.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_dialog.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_dropdown.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_dropdown.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_dropdown.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_edittext.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_edittext.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_edittext.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_graph.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_graph.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_graph.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_image.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_image.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_image.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_led.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_led.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_led.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_listbox.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_listbox.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_listbox.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_listview.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_listview.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_listview.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_progbar.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_progbar.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_progbar.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_radio.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_radio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_radio.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_slider.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_slider.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_slider.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_textview.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_textview.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_textview.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_widget.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_widget.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_widget.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI/Widgets/gui_window.o: C:/Users/tilen\ majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets/gui_window.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/tilen majerle/Documents/EasyGUI/01-DEV_RTOS/User" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/input" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/widgets" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/pt" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-GUI_LIBRARY/utils" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-STM32_LIBRARIES" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/tilen majerle/Documents/EasyGUI/00-HAL_DRIVERS/CMSIS/Device/ST/STM32F7xx/Include" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"GUI/Widgets/gui_window.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


