
# List of source files for the kernel.
set(KERNEL_SOURCES
    dir.c
    dispatch.c
    file.c
    ident.c
    inode.c
    kmalloc.c
    main.c
    mount.c
    printk.c
    proc.c
    sched.c
    sem.c
    strerror.c
    switcher.S
    uname.c
    util.c
    drivers/devices.c
    drivers/misc/misc.h
    drivers/misc/null.c
    drivers/sdcard/sdasm.S
    drivers/sdcard/sdcard.h
    drivers/sdcard/sdcard.c
    drivers/spi/spi.h
    drivers/tty/console.h
    drivers/tty/basictty.c
    fs/fat/fatdefs.h
    fs/fat/fatfs.h
    fs/fat/fatfs.c
    fs/fat/fatutils.h
    fs/fat/fatutils.c
    fs/ram/ramfs.h
    fs/ram/ramfs.c
)

# Build a kernel for "mos-sim" from the "llvm-mos" project.
add_executable(kernel-sim ${KERNEL_SOURCES})
add_dependencies(kernel-sim shell-sim)
target_include_directories(kernel-sim PUBLIC ${CMAKE_CURRENT_LIST_DIR})
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
add_executable(kernel-eater ${KERNEL_SOURCES})
add_dependencies(kernel-eater shell-eater)
target_include_directories(kernel-eater PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_compile_options(kernel-eater
    PUBLIC -DMOSNIX_TARGET_EATER=1
           -DMOSNIX_VERSION=\"${CMAKE_PROJECT_VERSION}\"
           -mcpu=mosw65c02 -DCPU_65C02
           -I${CMAKE_SOURCE_DIR}/target/eater
)
target_link_options(kernel-eater
    PRIVATE -mlto-zp=32 -T${CMAKE_SOURCE_DIR}/target/eater/link.ld
            -L${CMAKE_BINARY_DIR}/target/eater -ltarget-eater
            -mcpu=mosw65c02 -DCPU_65C02
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
