
# Build the main libc library for MOSnix user space applications.
add_library(libc-mosnix STATIC
    getchar.c
    putchar.c
    syscall.S
    unistd.c
)

# Build the startup.o file for MOSnix user space applications.
add_library(startup-mosnix STATIC
    startup.S
)
add_custom_command(
    OUTPUT startup.o
    COMMAND ${LLVM_MOS_LINKER} -r -o startup.o --whole-archive $<TARGET_FILE:startup-mosnix>
)
add_custom_target(startup-mosnix-o ALL DEPENDS startup.o)
