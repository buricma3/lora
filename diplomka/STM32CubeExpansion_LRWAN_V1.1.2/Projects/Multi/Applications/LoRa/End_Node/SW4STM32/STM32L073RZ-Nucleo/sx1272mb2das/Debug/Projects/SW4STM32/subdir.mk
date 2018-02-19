################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/SW4STM32/STM32L073RZ-Nucleo/startup_stm32l073xx.s 

OBJS += \
./Projects/SW4STM32/startup_stm32l073xx.o 


# Each subdirectory must supply rules for building sources it contributes
Projects/SW4STM32/startup_stm32l073xx.o: C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/SW4STM32/STM32L073RZ-Nucleo/startup_stm32l073xx.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


