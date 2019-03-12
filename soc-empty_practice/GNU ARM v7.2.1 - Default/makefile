################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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
all: soc-empty_practice.axf

# Tool invocations
soc-empty_practice.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "soc-empty_practice.ld" -nostdlib -Xlinker --gc-sections -Xlinker -Map="soc-empty_practice.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o soc-empty_practice.axf "./bsp_trace.o" "./retargetio.o" "./retargetserial.o" 
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: soc-empty_practice.hex'
	arm-none-eabi-objcopy -O ihex "soc-empty_practice.axf" "soc-empty_practice.hex"
	@echo ' '

	@echo 'Building bin file: soc-empty_practice.bin'
	arm-none-eabi-objcopy -O binary "soc-empty_practice.axf" "soc-empty_practice.bin"
	@echo ' '

	@echo 'Building s37 file: soc-empty_practice.s37'
	arm-none-eabi-objcopy -O srec "soc-empty_practice.axf" "soc-empty_practice.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "soc-empty_practice.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) soc-empty_practice.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
