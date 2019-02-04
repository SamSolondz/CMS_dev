################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../inc/em_core.c 

OBJS += \
./inc/em_core.o 

C_DEPS += \
./inc/em_core.d 


# Each subdirectory must supply rules for building sources it contributes
inc/em_core.o: ../inc/em_core.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG=1' '-DEFR32BG1B232F256GM48=1' -I"C:\Users\Sam Solondz\SimplicityStudio\v4_workspace\SPI_dev\SPI_test_adc\inc" -I"C:/Users/Sam Solondz/SimplicityStudio/v4_workspace/SPI_dev/SPI_test_adc/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFR32BG1B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"inc/em_core.d" -MT"inc/em_core.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


