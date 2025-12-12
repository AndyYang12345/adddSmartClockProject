################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lib/Src/addd_rtc.c \
../Lib/Src/blt.c \
../Lib/Src/buzzer.c \
../Lib/Src/lcd1602.c \
../Lib/Src/led.c \
../Lib/Src/matbtn.c 

OBJS += \
./Lib/Src/addd_rtc.o \
./Lib/Src/blt.o \
./Lib/Src/buzzer.o \
./Lib/Src/lcd1602.o \
./Lib/Src/led.o \
./Lib/Src/matbtn.o 

C_DEPS += \
./Lib/Src/addd_rtc.d \
./Lib/Src/blt.d \
./Lib/Src/buzzer.d \
./Lib/Src/lcd1602.d \
./Lib/Src/led.d \
./Lib/Src/matbtn.d 


# Each subdirectory must supply rules for building sources it contributes
Lib/Src/%.o Lib/Src/%.su Lib/Src/%.cyclo: ../Lib/Src/%.c Lib/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../App/Inc -I../Lib/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Lib-2f-Src

clean-Lib-2f-Src:
	-$(RM) ./Lib/Src/addd_rtc.cyclo ./Lib/Src/addd_rtc.d ./Lib/Src/addd_rtc.o ./Lib/Src/addd_rtc.su ./Lib/Src/blt.cyclo ./Lib/Src/blt.d ./Lib/Src/blt.o ./Lib/Src/blt.su ./Lib/Src/buzzer.cyclo ./Lib/Src/buzzer.d ./Lib/Src/buzzer.o ./Lib/Src/buzzer.su ./Lib/Src/lcd1602.cyclo ./Lib/Src/lcd1602.d ./Lib/Src/lcd1602.o ./Lib/Src/lcd1602.su ./Lib/Src/led.cyclo ./Lib/Src/led.d ./Lib/Src/led.o ./Lib/Src/led.su ./Lib/Src/matbtn.cyclo ./Lib/Src/matbtn.d ./Lib/Src/matbtn.o ./Lib/Src/matbtn.su

.PHONY: clean-Lib-2f-Src

