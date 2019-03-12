################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bsp_trace.c \
../retargetio.c \
../retargetserial.c 

OBJS += \
./bsp_trace.o \
./retargetio.o \
./retargetserial.o 

C_DEPS += \
./bsp_trace.d \
./retargetio.d \
./retargetserial.d 


# Each subdirectory must supply rules for building sources it contributes
bsp_trace.o: ../bsp_trace.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"bsp_trace.d" -MT"bsp_trace.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

retargetio.o: ../retargetio.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"retargetio.d" -MT"retargetio.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

retargetserial.o: ../retargetserial.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"retargetserial.d" -MT"retargetserial.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


