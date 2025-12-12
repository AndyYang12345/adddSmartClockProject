################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Src/alarm.c \
../App/Src/btn_response.c \
../App/Src/get_time.c \
../App/Src/task_lcd.c \
../App/Src/task_main.c \
../App/Src/test.c \
../App/Src/timer.c 

OBJS += \
./App/Src/alarm.o \
./App/Src/btn_response.o \
./App/Src/get_time.o \
./App/Src/task_lcd.o \
./App/Src/task_main.o \
./App/Src/test.o \
./App/Src/timer.o 

C_DEPS += \
./App/Src/alarm.d \
./App/Src/btn_response.d \
./App/Src/get_time.d \
./App/Src/task_lcd.d \
./App/Src/task_main.d \
./App/Src/test.d \
./App/Src/timer.d 


# Each subdirectory must supply rules for building sources it contributes
App/Src/%.o App/Src/%.su App/Src/%.cyclo: ../App/Src/%.c App/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../App/Inc -I../Lib/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-App-2f-Src

clean-App-2f-Src:
	-$(RM) ./App/Src/alarm.cyclo ./App/Src/alarm.d ./App/Src/alarm.o ./App/Src/alarm.su ./App/Src/btn_response.cyclo ./App/Src/btn_response.d ./App/Src/btn_response.o ./App/Src/btn_response.su ./App/Src/get_time.cyclo ./App/Src/get_time.d ./App/Src/get_time.o ./App/Src/get_time.su ./App/Src/task_lcd.cyclo ./App/Src/task_lcd.d ./App/Src/task_lcd.o ./App/Src/task_lcd.su ./App/Src/task_main.cyclo ./App/Src/task_main.d ./App/Src/task_main.o ./App/Src/task_main.su ./App/Src/test.cyclo ./App/Src/test.d ./App/Src/test.o ./App/Src/test.su ./App/Src/timer.cyclo ./App/Src/timer.d ./App/Src/timer.o ./App/Src/timer.su

.PHONY: clean-App-2f-Src

