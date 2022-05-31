BUILD_DIR = .

# C sources
C_SOURCES = \
	src/main.c

# ASM sources
# ASM_SOURCES = \
# 	src/startup.s
ASM_SOURCES = 

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DSTM32L011xx
# -DUSE_HAL_DRIVER \

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-Iinclude \
-ISMT32CubeL0/Drivers/STM32L0xx_HAL_Driver/Inc \
-ISMT32CubeL0/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy \
-ISMT32CubeL0/Drivers/CMSIS/Device/ST/STM32L0xx/Include \
-ISMT32CubeL0/Drivers/CMSIS/Include

CC=arm-none-eabi-gcc
AS=arm-none-eabi-gcc -x assembler-with-cpp
CP=arm-none-eabi-objcopy
SZ=arm-none-eabi-size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

MACH=cortex-m0plus
ASFLAGS = -mcpu=$(MACH) $(AS_DEFS) $(AS_INCLUDES) -Wall -mthumb -g -gdwarf-2
CFLAGS= -mcpu=$(MACH) $(C_DEFS) $(C_INCLUDES) -Wall -mthumb -std=gnu11 -O0 -g -gdwarf-2
LDSCRIPT=ls.ld
LDFLAGS= -T $(LDSCRIPT) -nostdlib -Wl,-Map=build/firmware.map
# -Wl,--gc-sections

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

all: build/firmware.elf

build/%.o: src/%.c
	mkdir -p build
	$(CC) -c $(CFLAGS) $^ -o $@

build/firmware.elf: build/main.o build/stm32l011k4_startup.o
	$(CC) $(LDFLAGS) $^ -o $@

flash: build/firmware.elf
	openocd -f interface/stlink.cfg -f target/stm32l0.cfg -c "program build/firmware.elf verify reset exit"

dis: firmware.elf
	arm-none-eabi-objdump -S build/firmware.elf > build/objdump_src.S
	arm-none-eabi-objdump -D build/firmware.elf > build/objdump_all.S
	arm-none-eabi-objdump -d build/firmware.elf > build/objdump.S
	arm-none-eabi-objdump -h build/firmware.elf > build/objdump_sections.txt
	nm --print-size --size-sort --radix=x build/firmware.elf > build/nm.txt
	arm-none-eabi-readelf -a build/firmware.elf > build/readelf_all.txt
	arm-none-eabi-size -G -d build/firmware.elf > build/size.txt

clean:
	# rm -rf *.o *.elf *.d
	rm -rf build

-include $(wildcard build/*.d)