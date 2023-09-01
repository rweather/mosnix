
# List of source files for the kernel.
set(KERNEL_SOURCES
    dispatch.c
    fd.c
    ident.c
    main.c
    notimp.c
    proc.c
    sched.c
    switcher.S
)

# Build a kernel for "mos-sim" from the "llvm-mos" project.
add_executable(mosnix-sim-${CMAKE_PROJECT_VERSION} ${KERNEL_SOURCES})
target_compile_options(mosnix-sim-${CMAKE_PROJECT_VERSION}
    PUBLIC -DMOSNIX_TARGET_SIM=1
           -DMOSNIX_VERSION=\"${CMAKE_PROJECT_VERSION}\"
           -I${CMAKE_SOURCE_DIR}/target/sim
)
target_link_options(mosnix-sim-${CMAKE_PROJECT_VERSION}
    PRIVATE -mlto-zp=32 -T${CMAKE_SOURCE_DIR}/target/sim/link.ld
            -L${CMAKE_BINARY_DIR}/target/sim -ltarget-sim
            -lexit-custom -linit-stack -lcopy-zp-data -lzero-bss
)

# Build a kernel for Ben Eater's breadboard computer.
add_executable(mosnix-eater-${CMAKE_PROJECT_VERSION} ${KERNEL_SOURCES})
target_compile_options(mosnix-eater-${CMAKE_PROJECT_VERSION}
    PUBLIC -mcpu=mosw65c02 -DCPU_65C02 -DMOSNIX_TARGET_EATER=1
           -DMOSNIX_VERSION=\"${CMAKE_PROJECT_VERSION}\"
           -I${CMAKE_SOURCE_DIR}/target/eater
)
target_link_options(mosnix-eater-${CMAKE_PROJECT_VERSION}
    PRIVATE -mlto-zp=32 -T${CMAKE_SOURCE_DIR}/target/eater/link.ld
            -L${CMAKE_BINARY_DIR}/target/eater -ltarget-eater
            -lexit-loop -linit-stack -lcopy-zp-data -lzero-bss
)

# Install all kernels under /usr/local/share/mosnix/kernels or equivalent.
install(TARGETS
    mosnix-sim-${CMAKE_PROJECT_VERSION}
    mosnix-eater-${CMAKE_PROJECT_VERSION}
DESTINATION ${CMAKE_INSTALL_DATADIR}/mosnix/kernels)
