BUILD_DIR = .

# C sources
C_SOURCES =  \
src/main.c \
src/stm32l011k4_startup.c \
STM32CubeL0/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c

# ASM sources
ASM_SOURCES =  \
src/sfunc.s

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DSTM32L011xx \
-DUSE_HAL_DRIVER

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-Iinclude \
-ISTM32CubeL0/Drivers/STM32L0xx_HAL_Driver/Inc \
-ISTM32CubeL0/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy \
-ISTM32CubeL0/Drivers/CMSIS/Device/ST/STM32L0xx/Include \
-ISTM32CubeL0/Drivers/CMSIS/Include

CC=arm-none-eabi-gcc
AS=arm-none-eabi-gcc -x assembler-with-cpp
CP=arm-none-eabi-objcopy
SZ=arm-none-eabi-size -d -G
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

CPU = -mcpu=cortex-m0plus -mthumb

CFLAGS= $(CPU) $(C_DEFS) $(C_INCLUDES) -Wall -fdata-sections -ffunction-sections -Og -g -gdwarf-2
ASFLAGS = $(CPU) $(AS_DEFS) $(AS_INCLUDES) -Wall -fdata-sections -ffunction-sections -Og -g -gdwarf-2

LDSCRIPT=ls.ld
LDFLAGS= $(CPU) -T$(LDSCRIPT) -lc -lm -lnosys -specs=nano.specs -Wl,-Map=build/firmware.map -Wl,--gc-sections
# -Wl,--gc-sections
# -nostdlib
# -Og : optimization option specially choosen for debug
# -O0 : no debug

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

all: build/firmware.elf

build/%.o: src/%.c
	mkdir -p build
	$(CC) -c $(CFLAGS) $^ -o $@

build/stm32l0xx_hal.o: STM32CubeL0/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c
	$(CC) -c $(CFLAGS) $^ -o $@

build/stm32l0xx_hal_cortex.o: STM32CubeL0/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c
	$(CC) -c $(CFLAGS) $^ -o $@

build/stm32l0xx_hal_rcc.o: STM32CubeL0/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c
	$(CC) -c $(CFLAGS) $^ -o $@

build/stm32l0xx_hal_rcc_ex.o: STM32CubeL0/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c
	$(CC) -c $(CFLAGS) $^ -o $@

build/%.s.o: src/%.s
	$(AS) -c $(CFLAGS) $^ -o $@

build/firmware.elf: build/main.o build/stm32l011k4_startup.o build/sfunc.s.o build/stm32l0xx_hal.o build/stm32l0xx_hal_rcc.o build/stm32l0xx_hal_cortex.o build/stm32l0xx_hal_rcc_ex.o
	$(CC) $(LDFLAGS) $^ -o $@
	$(SZ) $@

flash: build/firmware.elf
	openocd -f interface/stlink.cfg -f target/stm32l0.cfg -c "program build/firmware.elf verify reset exit"

dis: build/firmware.elf
	arm-none-eabi-objdump -S build/firmware.elf > build/objdump_src.S
	arm-none-eabi-objdump -D build/firmware.elf > build/objdump_all.S
	arm-none-eabi-objdump -d build/firmware.elf > build/objdump.S
	arm-none-eabi-objdump -h build/firmware.elf > build/objdump_sections.txt
	nm --print-size --size-sort --radix=x build/firmware.elf > build/nm.txt
	arm-none-eabi-readelf -a build/firmware.elf > build/readelf_all.txt
	arm-none-eabi-size -G -d build/firmware.elf > build/size.txt

qemu: build/firmware.elf
	qemu-arm -cpu cortex-m0 -singlestep -g 1234 build/firmware.elf

clean:
	# rm -rf *.o *.elf *.d
	rm -rf build

-include $(wildcard build/*.d)