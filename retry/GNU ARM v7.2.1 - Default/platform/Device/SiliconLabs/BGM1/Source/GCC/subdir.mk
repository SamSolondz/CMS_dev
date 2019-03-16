################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.c 

OBJS += \
./platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.o 

C_DEPS += \
./platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.d 


# Each subdirectory must supply rules for building sources it contributes
platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.o: ../platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DBGM111A256V2=1' '-DHAL_CONFIG=1' '-D__StackLimit=0x20000000' '-D__HEAP_SIZE=0xD00' '-D__STACK_SIZE=0x800' -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\emdrv\uartdrv\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\halconfig\inc\hal-config" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\app\bluetooth\common\util" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\bootloader\api" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\hardware\kit\common\drivers" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\radio\rail_lib\common" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\emdrv\tempdrv\src" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\emdrv\sleep\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\Device\SiliconLabs\BGM1\Source" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\protocol\bluetooth\ble_stack\inc\common" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\radio\rail_lib\chip\efr32\efr32xg1x" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\emdrv\common\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\protocol\bluetooth\ble_stack\inc\soc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\Device\SiliconLabs\BGM1\Source\GCC" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\emdrv\sleep\src" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\emlib\src" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\hardware\kit\BGM11_BRD4300A\config" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\emlib\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\bootloader" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\CMSIS\Include" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\Device\SiliconLabs\BGM1\Include" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\emdrv\tempdrv\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\hardware\module\config" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\platform\emdrv\gpiointerrupt\inc" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\hardware\kit\common\bsp" -I"C:\Users\Touzong\SimplicityStudio\v4_workspace\SPI_dev\retry\hardware\kit\common\halconfig" -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.d" -MT"platform/Device/SiliconLabs/BGM1/Source/GCC/startup_bgm1.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


