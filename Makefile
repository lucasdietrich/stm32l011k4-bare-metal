all: cmake make

cmake:
	cmake -DCMAKE_TOOLCHAIN_FILE=./toolchain-arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build

make:
	make -C build --no-print-directory

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

# Legacy commands
legacy:
	make --file=LegacyMakefile
	
legacy_dis: build/firmware.elf
	make --file=LegacyMakefile dis


clean:
	rm -rf build