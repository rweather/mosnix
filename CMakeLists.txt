cmake_minimum_required(VERSION 3.5)
include(CheckIncludeFiles)
include(CheckLibraryExists)

# Set the project name and version number.
project(mosnix VERSION 0.1.0 LANGUAGES C ASM)

# Set up the type of build and the compiler flags to use.
if(NOT CMAKE_BUILD_TYPE)
    if(COVERAGE)
        set(CMAKE_BUILD_TYPE Debug)
    else()
        set(CMAKE_BUILD_TYPE Release)
    endif()
endif()
set(CMAKE_C_FLAGS "-Wall -Wextra ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "-g -Os")
set(CMAKE_C_FLAGS_RELEASE "-Os")

# We need the llvm-mos tools to cross-compile the kernel and user space.
find_program(MOS_CLANG NAMES mos-common-clang REQUIRED)
find_program(LLVM_MOS_LINKER NAMES ld.lld REQUIRED)
find_program(LLVM_AR NAMES llvm-ar REQUIRED)
find_program(LLVM_RANLIB NAMES llvm-ranlib REQUIRED)
find_program(MOS_SIM NAMES mos-sim REQUIRED)

# We need "elf2o65" to convert .elf files into .o65 format.
find_program(ELF2O65 NAMES elf2o65 REQUIRED)

# Set up the cross-compilation toolchain.
set(CMAKE_SYSTEM_NAME mosnix)
set(CMAKE_SYSTEM_PROCESSOR mos)
set(CMAKE_C_COMPILER ${MOS_CLANG})
set(CMAKE_ASM_COMPILER ${MOS_CLANG})
set(CMAKE_AR ${LLVM_AR})
set(CMAKE_RANLIB ${LLVM_RANLIB})

# Set up the main include directory.
include_directories(include)

# Add the subdirectories.
add_subdirectory(include)
add_subdirectory(target)
add_subdirectory(libc-mosnix)
add_subdirectory(shell)
add_subdirectory(os)

# "make boot" will boot the kernel with "mos-sim".
add_custom_target(boot COMMAND
    ${MOS_SIM} --cmos ${CMAKE_BINARY_DIR}/os/mosnix-sim
)
