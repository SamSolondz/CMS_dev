################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../soc-ibeacon/platform/Device/SiliconLabs/EFR32MG1P/Source/system_efr32mg1p.c 

OBJS += \
./soc-ibeacon/platform/Device/SiliconLabs/EFR32MG1P/Source/system_efr32mg1p.o 

C_DEPS += \
./soc-ibeacon/platform/Device/SiliconLabs/EFR32MG1P/Source/system_efr32mg1p.d 


# Each subdirectory must supply rules for building sources it contributes
soc-ibeacon/platform/Device/SiliconLabs/EFR32MG1P/Source/system_efr32mg1p.o: ../soc-ibeacon/platform/Device/SiliconLabs/EFR32MG1P/Source/system_efr32mg1p.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DBGM111A256V2=1' '-DHAL_CONFIG=1' '-D__STACK_SIZE=0x800' '-D__HEAP_SIZE=0xD00' '-D__StackLimit=0x20000000' -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\radio\rail_lib\common" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\emdrv\tempdrv\src" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\emlib\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\bootloader\api" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\hardware\kit\common\halconfig" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\hardware\module\config" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\CMSIS\Include" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\Device\SiliconLabs\BGM1\Include" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\protocol\bluetooth\ble_stack\inc\common" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\emlib\src" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\hardware\kit\BGM11_BRD4300A\config" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\halconfig\inc\hal-config" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\emdrv\sleep\src" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\hardware\kit\common\bsp" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\radio\rail_lib\chip\efr32\efr32xg1x" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\emdrv\tempdrv\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\hardware\kit\common\drivers" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\protocol\bluetooth\ble_stack\inc\soc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\emdrv\uartdrv\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\emdrv\gpiointerrupt\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\app\bluetooth\common\util" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\emdrv\common\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\emdrv\sleep\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\Device\SiliconLabs\BGM1\Source" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\Device\SiliconLabs\BGM1\Source\GCC" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\soc-empty_practice\platform\bootloader" -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"soc-ibeacon/platform/Device/SiliconLabs/EFR32MG1P/Source/system_efr32mg1p.d" -MT"soc-ibeacon/platform/Device/SiliconLabs/EFR32MG1P/Source/system_efr32mg1p.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


