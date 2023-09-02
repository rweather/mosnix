System call interface for MOSnix
================================

The general approach of the system call interface of MOSnix is to let the
[llvm-mos C calling conventions](https://llvm-mos.org/wiki/C_calling_convention)
do most of the heavy lifting.

The llvm-mos compiler uses 32 _imaginary registers_ `rc0` ... `rc31` to
pass parameters to functions, return results, and store local variables.
If the compiler runs out of imaginary registers, then it will pass extra
parameters on the C stack (which is not the same as the 6502's
return address stack).

The first parameter to a function is passed in the A:X register pair
if a number or the `rc2:rc3` imaginary register pair if a pointer.
The top of stack is pointed to by the imaginary register pair `rc0:rc1`.

The kernel and the user space processes each have their own separate
copy of the imaginary registers and the stack.  A method is needed to
pass values back and forth across the system call boundary without
imposing too much overhead.

The `syscall()` function declared in `<sys/syscall.h>` has the
following prototype:

    int syscall(unsigned char number, ...);

The llvm-mos C calling conventions will arrange for all of the arguments
in the ellipsis `...` to be passed on the C stack.  The top of C
stack is pointed to by the imaginary register pair `rc0:rc1`
and the system call number will be in the A register.

The `syscall()` function is used as follows to define system call
wrapper functions:

    ssize_t read(int fd, void *data, size_t size)
    {
        return syscall(SYS_read, fd, data, size);
    }

Behind the scenes, llvm-mos arranges to set A to `SYS_read` and to
place the arguments on the C stack.  The `syscall()` function then
performs the following pseudocode to call into the kernel:

    Y = A * 2
    A:X = rc0:rc1
    BRK
    if (A:X < 0) {
        errno = -A:X
        return -1
    } else {
        return A:X
    }

The system call number is doubled and copied into Y.  The doubling helps the
kernel by making Y an offset into the system call dispatch table.  If we
leave this to later, then more register juggling is required inside the kernel.
It is easier to make the user space application do the doubling.

The top of stack pointer in `rc0:rc1` is transferred to A:X
so that it can be passed to the kernel system call as a pointer to a
`struct` containing the parameters.

The `BRK` instruction is used to execute a system call trap into the kernel.
This may be replaced by a more direct function call into the kernel later.

Inside the kernel, the system call is implemented as follows:

    struct sys_read_s {
        int fd;
        void *data;
        size_t size;
    };

    int sys_read(struct sys_read_s *args)
    {
        ...
    }

The kernel can use its own imaginary registers and C stack to implement
the system call.  The llvm-mos handling of variadic functions has already
packed the values into a `struct` for the system call.  The only thing
that is shared is the 6502's return address stack.

System calls that return results other than an `int` must pass a
pointer to a return variable to the kernel.  For example:

    off_t lseek(int fd, off_t offset, int whence)
    {
        off_t result;
        if (syscall(SYS_lseek, fd, offset, whence, &result) == 0)
            return result;
        else
            return -1;
    }

Copyright (c) 2023 Rhys Weatherley

Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
See [https://github.com/rweater/mosnix/blob/main/LICENSE](https://github.com/rweater/mosnix/blob/main/LICENSE)
for license information.
