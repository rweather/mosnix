
# List of source files for the kernel.
set(KERNEL_SOURCES
    dispatch.c
    file.c
    ident.c
    kmalloc.c
    main.c
    notimp.c
    printk.c
    proc.c
    sched.c
    sem.c
    switcher.S
    uname.c
    drivers/devices.h
    drivers/devices.c
    drivers/misc/misc.h
    drivers/misc/null.c
    drivers/tty/console.h
    drivers/tty/basictty.c
)

# Build a kernel for "mos-sim" from the "llvm-mos" project.
add_executable(kernel-sim ${KERNEL_SOURCES})
add_dependencies(kernel-sim shell-sim)
target_compile_options(kernel-sim
    PUBLIC -DMOSNIX_TARGET_SIM=1
           -DMOSNIX_VERSION=\"${CMAKE_PROJECT_VERSION}\"
           -I${CMAKE_SOURCE_DIR}/target/sim
)
target_link_options(kernel-sim
    PRIVATE -mlto-zp=32 -T${CMAKE_SOURCE_DIR}/target/sim/link.ld
            -L${CMAKE_BINARY_DIR}/target/sim -ltarget-sim
            -lexit-custom -linit-stack -lcopy-zp-data -lzero-bss
)
add_custom_target(mosnix-sim ALL
    COMMAND cat ${CMAKE_BINARY_DIR}/shell/shell-sim kernel-sim >mosnix-sim
    DEPENDS ${CMAKE_BINARY_DIR}/shell/shell-sim
)
add_dependencies(mosnix-sim kernel-sim shell-sim)

# Build a kernel for Ben Eater's breadboard computer.
#
# llvm-mos has some issues with function pointers on 65c02 so for now
# we use basic 6502 compilation until this can be fixed.
#
#       -mcpu=mosw65c02 -DCPU_65C02
#
add_executable(kernel-eater ${KERNEL_SOURCES})
add_dependencies(kernel-eater shell-eater)
target_compile_options(kernel-eater
    PUBLIC -DMOSNIX_TARGET_EATER=1
           -DMOSNIX_VERSION=\"${CMAKE_PROJECT_VERSION}\"
           -I${CMAKE_SOURCE_DIR}/target/eater
)
target_link_options(kernel-eater
    PRIVATE -mlto-zp=32 -T${CMAKE_SOURCE_DIR}/target/eater/link.ld
            -L${CMAKE_BINARY_DIR}/target/eater -ltarget-eater
            -lexit-loop -linit-stack -lcopy-zp-data -lzero-bss
)
add_custom_target(mosnix-eater ALL
    COMMAND cat ${CMAKE_BINARY_DIR}/shell/shell-eater kernel-eater >mosnix-eater
    DEPENDS ${CMAKE_BINARY_DIR}/shell/shell-eater
)
add_dependencies(mosnix-eater kernel-eater shell-eater)

# Install the kernels under "<prefix>/share/mosnix/kernels".
install(FILES
    mosnix-sim
    mosnix-eater
DESTINATION ${CMAKE_INSTALL_DATADIR}/mosnix/kernels)
