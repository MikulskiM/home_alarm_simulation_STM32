################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/gpio.c \
../Src/i2c.c \
../Src/lcd_i2c.c \
../Src/main.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f4xx.c \
../Src/tim.c 

OBJS += \
./Src/gpio.o \
./Src/i2c.o \
./Src/lcd_i2c.o \
./Src/main.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f4xx.o \
./Src/tim.o 

C_DEPS += \
./Src/gpio.d \
./Src/i2c.d \
./Src/lcd_i2c.d \
./Src/main.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f4xx.d \
./Src/tim.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_HAL_DRIVER -DSTM32F411xE -I"D:/STM_projects/SYSTEM_ALARMOWY_v3/Inc" -I"D:/STM_projects/SYSTEM_ALARMOWY_v3/Drivers/STM32F4xx_HAL_Driver/Inc" -I"D:/STM_projects/SYSTEM_ALARMOWY_v3/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"D:/STM_projects/SYSTEM_ALARMOWY_v3/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"D:/STM_projects/SYSTEM_ALARMOWY_v3/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


