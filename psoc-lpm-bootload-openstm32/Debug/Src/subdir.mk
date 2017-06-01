################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/comms.c \
../Src/cybtldr_api.c \
../Src/cybtldr_api2.c \
../Src/cybtldr_command.c \
../Src/cybtldr_parse.c \
../Src/main.c \
../Src/psocbootloader.c \
../Src/stm32f3xx_hal_msp.c \
../Src/stm32f3xx_it.c \
../Src/system_stm32f3xx.c 

OBJS += \
./Src/comms.o \
./Src/cybtldr_api.o \
./Src/cybtldr_api2.o \
./Src/cybtldr_command.o \
./Src/cybtldr_parse.o \
./Src/main.o \
./Src/psocbootloader.o \
./Src/stm32f3xx_hal_msp.o \
./Src/stm32f3xx_it.o \
./Src/system_stm32f3xx.o 

C_DEPS += \
./Src/comms.d \
./Src/cybtldr_api.d \
./Src/cybtldr_api2.d \
./Src/cybtldr_command.d \
./Src/cybtldr_parse.d \
./Src/main.d \
./Src/psocbootloader.d \
./Src/stm32f3xx_hal_msp.d \
./Src/stm32f3xx_it.d \
./Src/system_stm32f3xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F303xE -I"/scratch/work/eddie/psoc/psoc-lpm-openstm32/Inc" -I"/scratch/work/eddie/psoc/psoc-lpm-openstm32/Drivers/STM32F3xx_HAL_Driver/Inc" -I"/scratch/work/eddie/psoc/psoc-lpm-openstm32/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy" -I"/scratch/work/eddie/psoc/psoc-lpm-openstm32/Drivers/CMSIS/Device/ST/STM32F3xx/Include" -I"/scratch/work/eddie/psoc/psoc-lpm-openstm32/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


