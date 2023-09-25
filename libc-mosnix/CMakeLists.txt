
# Build the main libc library for MOSnix user space applications.
add_library(libc-mosnix STATIC
    dirent.c
    fcntl.c
    getchar.c
    getopt.c
    mkdir.c
    open.c
    putchar.c
    stat.c
    strerror.c
    syscall.S
    uname.c
    unistd.c
    yield.c
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
