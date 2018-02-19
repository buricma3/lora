################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/STM32L0xx_Nucleo/stm32l0xx_nucleo.c 

OBJS += \
./Drivers/BSP/STM32L0xx_Nucleo/stm32l0xx_nucleo.o 

C_DEPS += \
./Drivers/BSP/STM32L0xx_Nucleo/stm32l0xx_nucleo.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32L0xx_Nucleo/stm32l0xx_nucleo.o: C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/STM32L0xx_Nucleo/stm32l0xx_nucleo.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L073xx -DARM_MATH_CM0PLUS -DUSE_STM32L0XX_NUCLEO -DUSE_HAL_DRIVER -DREGION_EU868 -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/STM32L0xx_Nucleo" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1272" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"C:/Users/matula/Documents/GitHub/diplomka/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/sx1272mb2das"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

