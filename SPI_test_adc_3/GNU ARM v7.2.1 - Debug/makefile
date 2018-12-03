################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include CMSIS/EFR32BG1B/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: SPI_test_adc_3.axf

# Tool invocations
SPI_test_adc_3.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "SPI_test_adc_3.ld" -Xlinker --gc-sections -Xlinker -Map="SPI_test_adc_3.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o SPI_test_adc_3.axf "./CMSIS/EFR32BG1B/startup_efr32bg1b.o" "./CMSIS/EFR32BG1B/system_efr32bg1b.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_gpio.o" "./emlib/em_ldma.o" "./emlib/em_rtc.o" "./emlib/em_rtcc.o" "./emlib/em_system.o" "./emlib/em_usart.o" "./src/dmadrv.o" "./src/main.o" "./src/rtcdriver.o" "./src/spidrv.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: SPI_test_adc_3.hex'
	arm-none-eabi-objcopy -O ihex "SPI_test_adc_3.axf" "SPI_test_adc_3.hex"
	@echo ' '

	@echo 'Building bin file: SPI_test_adc_3.bin'
	arm-none-eabi-objcopy -O binary "SPI_test_adc_3.axf" "SPI_test_adc_3.bin"
	@echo ' '

	@echo 'Building s37 file: SPI_test_adc_3.s37'
	arm-none-eabi-objcopy -O srec "SPI_test_adc_3.axf" "SPI_test_adc_3.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "SPI_test_adc_3.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) SPI_test_adc_3.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
