################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Sam/SimplicityStudio/v4_workspace/SPI_dev/soc-empty_practice/platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.c 

OBJS += \
./platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.o 

C_DEPS += \
./platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.d 


# Each subdirectory must supply rules for building sources it contributes
platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.o: C:/Users/Sam/SimplicityStudio/v4_workspace/SPI_dev/soc-empty_practice/platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.d" -MT"platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


