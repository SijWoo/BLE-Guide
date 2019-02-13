################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include protocol/bluetooth/lib/EFR32BG13P/GCC/subdir.mk
-include platform/emlib/src/subdir.mk
-include platform/emdrv/sleep/src/subdir.mk
-include platform/Device/SiliconLabs/BGM13/Source/GCC/subdir.mk
-include platform/Device/SiliconLabs/BGM13/Source/subdir.mk
-include hardware/kit/common/drivers/subdir.mk
-include hardware/kit/common/bsp/subdir.mk
-include app/bluetooth/common/util/subdir.mk
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
all: BLE-soc-basic.axf

# Tool invocations
BLE-soc-basic.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\bgm13s32f512ga.ld" -Xlinker --gc-sections -Xlinker -Map="BLE-soc-basic.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o BLE-soc-basic.axf -Wl,--start-group "./app/bluetooth/common/util/infrastructure.o" "./app.o" "./application_properties.o" "./gatt_db.o" "./init_app.o" "./init_board_efr32xg1.o" "./init_mcu_efr32xg1.o" "./main.o" "./pti.o" "./hardware/kit/common/bsp/bsp_stk.o" "./hardware/kit/common/drivers/mx25flash_spi.o" "./hardware/kit/common/drivers/retargetio.o" "./hardware/kit/common/drivers/retargetserial.o" "./platform/Device/SiliconLabs/BGM13/Source/GCC/startup_bgm13.o" "./platform/Device/SiliconLabs/BGM13/Source/system_bgm13.o" "./platform/emdrv/sleep/src/sleep.o" "./platform/emlib/src/em_adc.o" "./platform/emlib/src/em_assert.o" "./platform/emlib/src/em_burtc.o" "./platform/emlib/src/em_cmu.o" "./platform/emlib/src/em_core.o" "./platform/emlib/src/em_cryotimer.o" "./platform/emlib/src/em_crypto.o" "./platform/emlib/src/em_emu.o" "./platform/emlib/src/em_gpio.o" "./platform/emlib/src/em_i2c.o" "./platform/emlib/src/em_msc.o" "./platform/emlib/src/em_rmu.o" "./platform/emlib/src/em_rtcc.o" "./platform/emlib/src/em_se.o" "./platform/emlib/src/em_system.o" "./platform/emlib/src/em_timer.o" "./platform/emlib/src/em_usart.o" "C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\radio\rail_lib\autogen\librail_release\librail_module_efr32xg13_gcc_release.a" "C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\protocol\bluetooth\lib\EFR32BG13P\GCC\libbluetooth.a" "C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\protocol\bluetooth\lib\EFR32BG13P\GCC\binapploader.o" "C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\protocol\bluetooth\lib\EFR32BG13P\GCC\libmbedtls.a" "C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\platform\radio\rail_lib\autogen\librail_release\librail_config_bgm13s32f512ga_gcc.a" "C:\Users\Sijin\Documents\SiLabs\BLE-Guide\BLE-soc-basic\protocol\bluetooth\lib\EFR32BG13P\GCC\libpsstore.a" -lm -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: BLE-soc-basic.hex'
	arm-none-eabi-objcopy -O ihex "BLE-soc-basic.axf" "BLE-soc-basic.hex"
	@echo ' '

	@echo 'Building bin file: BLE-soc-basic.bin'
	arm-none-eabi-objcopy -O binary "BLE-soc-basic.axf" "BLE-soc-basic.bin"
	@echo ' '

	@echo 'Building s37 file: BLE-soc-basic.s37'
	arm-none-eabi-objcopy -O srec "BLE-soc-basic.axf" "BLE-soc-basic.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "BLE-soc-basic.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) BLE-soc-basic.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
