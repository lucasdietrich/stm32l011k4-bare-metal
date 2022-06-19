cmake_minimum_required(VERSION 3.2)

project(stm32l011k4-bare-metal)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Important to keep ASM files
enable_language(C ASM)

###############################################################################

# Check that the target is statically linked (i.e. "-rdynamic" not set)

if(NOT CMAKE_SHARED_LIBRARY_LINK_C_FLAGS STREQUAL "")
	# message(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS: ${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS})
	set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
endif()

if(NOT CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS STREQUAL "")
	# message(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS: ${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS})
	set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
endif()

# Should be STATIC_LIBRARY
# message(${CMAKE_TRY_COMPILE_TARGET_TYPE})

###############################################################################

# set(CMAKE_BUILD_TYPE "RelDebugease") # or "Release"

set(EXE_NAME "firmware")
set(EXE "${EXE_NAME}.elf")

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/ls.ld")

message(s: ${CMAKE_EXE_LINKER_FLAGS})

# architecture
set(CPU "cortex-m0plus")

set(SZ "${CMAKE_SIZE_UTIL}")
set(OBJCOPY ${CMAKE_OBJCOPY_UTIL})

set(ELF_PATH "${CMAKE_BINARY_DIR}/${EXE}")

# CC=arm-none-eabi-gcc
# AS=arm-none-eabi-gcc -x assembler-with-cpp
# CP=arm-none-eabi-objcopy
# SZ=arm-none-eabi-size -d -G

###############################################################################

## STM32Cube Drivers Sources
set(STM32L0CUBE_PATH "${CMAKE_SOURCE_DIR}/STM32CubeL0")
set(STM32L0_HAL_DRIVER_PATH "${STM32L0CUBE_PATH}/Drivers/STM32L0xx_HAL_Driver")
set(STM32L0_HAL_DRIVER_INCLUDES_PATH "${STM32L0_HAL_DRIVER_PATH}/Inc")

file(GLOB STM32_HAL_DRIVERS_ALL_SOURCES "${STM32L0_HAL_DRIVER_PATH}/Src/*.c")

# Filter "template.c" files out of the list
FOREACH(item ${STM32_HAL_DRIVERS_ALL_SOURCES})
  STRING(REGEX MATCH "(.*)template\.c$" match ${item})
  IF(match STREQUAL "")
    LIST(APPEND STM32_HAL_DRIVERS_SOURCES ${item})
  ENDIF(match STREQUAL "")
ENDFOREACH(item ${STM32_HAL_DRIVERS_ALL_SOURCES})


## Project sources
file(GLOB_RECURSE C_SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE ASM_SOURCES "${CMAKE_SOURCE_DIR}/src/*.s")

message(${ASM_SOURCES})

## Target
add_executable(${EXE} ${C_SOURCES} ${ASM_SOURCES} ${STM32_HAL_DRIVERS_SOURCES})

target_compile_definitions(${EXE} PRIVATE
	-DSTM32L011xx
	-DUSE_HAL_DRIVER
)

target_include_directories(${EXE} PRIVATE
	# Drivers
	"${STM32L0_HAL_DRIVER_INCLUDES_PATH}"
	"${STM32L0_HAL_DRIVER_INCLUDES_PATH}/Legacy"

	# CMSIS
	"${STM32L0CUBE_PATH}/Drivers/CMSIS/Device/ST/STM32L0xx/Include"
	"${STM32L0CUBE_PATH}/Drivers/CMSIS/Include"
	"${STM32L0CUBE_PATH}/Drivers/BSP/STM32L0xx_Nucleo_32"

	# Application
	"${CMAKE_SOURCE_DIR}/include"
)

target_compile_options(${EXE} PRIVATE
	-mcpu=${CPU}
	-mthumb

	-fdata-sections
	-ffunction-sections

	-Wall

	# $<$<CONFIG:Debug>:-Og -g -gdwarf-2>
)

target_link_options(${EXE} PRIVATE
	-mcpu=${CPU}
	-mthumb

	-T${LINKER_SCRIPT}

	-lc
	-lm
	-lnosys
	
	# already defined in the toolchain
	# -specs=nano.specs
	-Wl,-Map=${EXE_NAME}.map,--cref
	-Wl,--gc-sections
	-Wl,--print-memory-usage
)

# PRE_BUILD, PRE_LINK, POST_BUILD

# Print size of executable
add_custom_command(TARGET ${EXE} POST_BUILD
        COMMAND ${SZ} -G -d ${EXE} > ${EXE_NAME}.size.txt
)

# Convert to hex & bin
add_custom_command(TARGET ${EXE} POST_BUILD
        COMMAND ${OBJCOPY} -O ihex ${EXE} ${EXE_NAME}.hex
        COMMAND ${OBJCOPY} -O binary -S ${EXE} ${EXE_NAME}.bin)