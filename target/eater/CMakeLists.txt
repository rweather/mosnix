
add_library(target-eater STATIC
    reset.S
    serial.S
    systick.S
    target.c
)
target_compile_options(target-eater PUBLIC -mcpu=mosw65c02)
