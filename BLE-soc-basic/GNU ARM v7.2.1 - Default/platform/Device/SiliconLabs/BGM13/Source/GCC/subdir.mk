################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../platform/Device/SiliconLabs/BGM13/Source/GCC/startup_bgm13.c 

OBJS += \
./platform/Device/SiliconLabs/BGM13/Source/GCC/startup_bgm13.o 

C_DEPS += \
./platform/Device/SiliconLabs/BGM13/Source/GCC/startup_bgm13.d 


# Each subdirectory must supply rules for building sources it contributes
platform/Device/SiliconLabs/BGM13/Source/GCC/startup_bgm13.o: ../platform/Device/SiliconLabs/BGM13/Source/GCC/startup_bgm13.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-D__HEAP_SIZE=0xD00' '-D__STACK_SIZE=0x800' '-DHAL_CONFIG=1' '-D__StackLimit=0x20000000' '-DBGM13S32F512GA=1' -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\CMSIS\Include" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\Device\SiliconLabs\BGM13\Include" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\hardware\module\config" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\emdrv\sleep\inc" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\emlib\inc" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\emlib\src" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\hardware\kit\common\drivers" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\Device\SiliconLabs\BGM13\Source\GCC" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\radio\rail_lib\protocol\ble" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\app\bluetooth\common\util" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\protocol\bluetooth\ble_stack\inc\common" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\hardware\kit\common\bsp" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\emdrv\common\inc" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\radio\rail_lib\protocol\ieee802154" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\radio\rail_lib\chip\efr32\efr32xg1x" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\hardware\kit\common\halconfig" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\radio\rail_lib\common" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\bootloader\api" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\Device\SiliconLabs\BGM13\Source" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\protocol\bluetooth\ble_stack\inc\soc" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\emdrv\uartdrv\inc" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\emdrv\gpiointerrupt\inc" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\emdrv\sleep\src" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\hardware\kit\BGM13_BRD4305A\config" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\halconfig\inc\hal-config" -I"C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\bootloader" -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"platform/Device/SiliconLabs/BGM13/Source/GCC/startup_bgm13.d" -MT"platform/Device/SiliconLabs/BGM13/Source/GCC/startup_bgm13.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


