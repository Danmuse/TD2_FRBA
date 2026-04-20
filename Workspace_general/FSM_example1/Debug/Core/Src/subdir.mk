################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/activeobject.c \
../Core/Src/application.c \
../Core/Src/assert_hal.c \
../Core/Src/eventqueue.c \
../Core/Src/led1.c \
../Core/Src/led2.c \
../Core/Src/main.c \
../Core/Src/statemachine.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/utils.c 

OBJS += \
./Core/Src/activeobject.o \
./Core/Src/application.o \
./Core/Src/assert_hal.o \
./Core/Src/eventqueue.o \
./Core/Src/led1.o \
./Core/Src/led2.o \
./Core/Src/main.o \
./Core/Src/statemachine.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/utils.o 

C_DEPS += \
./Core/Src/activeobject.d \
./Core/Src/application.d \
./Core/Src/assert_hal.d \
./Core/Src/eventqueue.d \
./Core/Src/led1.d \
./Core/Src/led2.d \
./Core/Src/main.d \
./Core/Src/statemachine.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/activeobject.cyclo ./Core/Src/activeobject.d ./Core/Src/activeobject.o ./Core/Src/activeobject.su ./Core/Src/application.cyclo ./Core/Src/application.d ./Core/Src/application.o ./Core/Src/application.su ./Core/Src/assert_hal.cyclo ./Core/Src/assert_hal.d ./Core/Src/assert_hal.o ./Core/Src/assert_hal.su ./Core/Src/eventqueue.cyclo ./Core/Src/eventqueue.d ./Core/Src/eventqueue.o ./Core/Src/eventqueue.su ./Core/Src/led1.cyclo ./Core/Src/led1.d ./Core/Src/led1.o ./Core/Src/led1.su ./Core/Src/led2.cyclo ./Core/Src/led2.d ./Core/Src/led2.o ./Core/Src/led2.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/statemachine.cyclo ./Core/Src/statemachine.d ./Core/Src/statemachine.o ./Core/Src/statemachine.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/utils.cyclo ./Core/Src/utils.d ./Core/Src/utils.o ./Core/Src/utils.su

.PHONY: clean-Core-2f-Src

