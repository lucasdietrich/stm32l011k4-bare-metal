BUILD_DIR = .

# C sources
C_SOURCES =  \
src/main.c \
src/stm32l011k4_startup.c \
STM32CubeL0/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c \
STM32CubeL0/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c \
STM32CubeL0/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c

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
-ISTM32CubeL0/Drivers/CMSIS/Include \
-ISTM32CubeL0/Drivers/BSP/STM32L0xx_Nucleo_32

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

OBJECTS = $(addprefix build/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix build/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

build:
	mkdir -p build

# pipe
build/%.o: %.c | build
	$(CC) -c $(CFLAGS) $< -o $@

build/%.o: %.s
	$(AS) -c $(CFLAGS) $< -o $@

build/firmware.elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	$(SZ) $@

dis: build/firmware.elf
	arm-none-eabi-objdump -S build/firmware.elf > build/objdump_src.S
	arm-none-eabi-objdump -D build/firmware.elf > build/objdump_all.S
	arm-none-eabi-objdump -d build/firmware.elf > build/objdump.S
	arm-none-eabi-objdump -h build/firmware.elf > build/objdump_sections.txt
	nm --print-size --size-sort --radix=x build/firmware.elf > build/nm.txt
	arm-none-eabi-readelf -a build/firmware.elf > build/readelf_all.txt
	arm-none-eabi-size -G -d build/firmware.elf > build/size.txt

flash: build/firmware.elf
	openocd -f interface/stlink.cfg -f target/stm32l0.cfg -c "program build/firmware.elf verify reset exit"

qemu: build/firmware.elf
	qemu-arm -cpu cortex-m0 -singlestep -g 1234 build/firmware.elf

clean:
	rm -rf build

-include $(wildcard build/*.d)