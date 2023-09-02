MOSnix
======

This repository contains an experiment in writing a POSIX-ish operating
system for 6502 microprocessors in C using [llvm-mos](https://llvm-mos.org/).

Why?
----

This project started with some observations.  The C programming language
was invented to implement Unix and we now have a decent C compiler for 6502,
llvm-mos.  So, can we implement an operating system in C using llvm-mos,
just like the original Unix?  How much of POSIX could we do?

Note that this isn't a port of someone else's fully fledged Unix.
And obviously it will be nowhere near as capable as Linux and other
modern OS designs in the Unix family.  There will of course be
shortcomings due to limited memory and no MMU.

There have been many attempts over the decades to create a Unix-like OS
for 6502.  But due to the difficulty of high-level programming on the 6502,
they have all fallen back on writing the OS and user space applications
in assembly.  This limits the power of the resulting system and can be
difficult to maintain.

Previous OS efforts have also been hampered by the hardware they run on.
For example, the Commodore 64's 1541 disk drives are notoriously slow.
Loading and running programs in something like
[GeckOS](http://www.6502.org/users/andre/osa/index.html) can be painful
because of this.  Even in emulation.

However, in recent years there have been an number of bare bones 6502
computers, notably [Ben Eater's Breadboard 6502](https://eater.net/6502).
There is very little legacy holding these designs back, other than the
lack of peripherals and good code.  Add a serial port as a console
and an SD card and the result is quite usable in a "1970's Unix"
kind of way.

This experiment is about finding out if a useful operating system can
be built in mostly C using llvm-mos, using a small amount of assembly
code for context switching and hardware interfacing.  The experiment
may fail, but I think it is worth doing.

What state is it in?
--------------------

Very early days!

* Basic system call dispatching to prove the concept.
* No more than 6 user space processes, 5 excluding the shell.
* A single user space process for the shell.
* Pre-emption has not been implemented yet.
* No filesystem, fork/exec, etc from POSIX.
* Reading from stdin and writing to stdout/stderr basically works.
* Shell doesn't do anything yet other than echo its input.
* System call numbering is not set in stone, will probably change.

Building
--------

You will need the following to build MOSnix:

* [LLVM-MOS SDK](https://github.com/llvm-mos/llvm-mos-sdk#getting-started),
with the `bin` directory somewhere on your `PATH`.
* `elf2o65` utility from [o65utils](https://github.com/rweather/o65utils)
for building relocatable user space application binaries.  This must also
be somewhere on your `PATH`.

Then configure and build the code as follows:

    mkdir build
    cd build
    cmake ..
    make

Two kernels are built by the default configuration, for the `mos-sim`
emulator, and the Ben Eater Breadboard 6502 computer.  After building,
you can run the kernel in the emulator with one of the following:

    make boot
    mos-sim build/os/mosnix-sim

To run it on a breadboard computer, you will need some modifications
for the serial port and SD card.  More information coming on this soon.

Contact
-------

For more information on this code, to report bugs, or to suggest
improvements, please contact the author Rhys Weatherley via
[email](mailto:rhys.weatherley@gmail.com).
