
# List of source files for the shell.
set(SHELL_SOURCES
    cd.c
    command.h
    command.c
    div10.S
    fstab.h
    ls.c
    main.c
    mount.c
    print.c
    print.h
    pwd.c
    rootfs.c
    uname.c
)

# Build a shell for "mos-sim" from the "llvm-mos" project.
add_executable(shell-sim ${SHELL_SOURCES})
target_compile_options(shell-sim
    PUBLIC -mcpu=mos6502 -DMOSNIX_TARGET_SIM=1
)
target_link_options(shell-sim
    PRIVATE -mlto-zp=0 -T${CMAKE_CURRENT_LIST_DIR}/link-sim.ld
            -L${CMAKE_BINARY_DIR}/libc-mosnix -llibc-mosnix
            -lexit-return -linit-stack -lcopy-zp-data -lzero-bss
            -mcpu=mos6502 ${CMAKE_BINARY_DIR}/libc-mosnix/startup.o
)

# Build a shell for Ben Eater's breadboard computer.
add_executable(shell-eater ${SHELL_SOURCES})
target_compile_options(shell-eater
    PUBLIC -mcpu=mosw65c02 -DMOSNIX_TARGET_EATER=1
)
target_link_options(shell-eater
    PRIVATE -mlto-zp=0 -T${CMAKE_CURRENT_LIST_DIR}/link-eater.ld
            -L${CMAKE_BINARY_DIR}/libc-mosnix -llibc-mosnix
            -lexit-return -linit-stack -lcopy-zp-data -lzero-bss
            -mcpu=mosw65c02 ${CMAKE_BINARY_DIR}/libc-mosnix/startup.o
)
