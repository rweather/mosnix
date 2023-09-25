Shell for MOSnix
================

The shell for MOSnix is built into the ROM image and runs as process 1
in place of "init" from other POSIX-compatible operating systems.

Builtin commands
----------------

Many of the traditional "shellutils" are built into the shell for performance.
The overhead of fork/exec to do "ls" or "rm" is simply too great.  Memory is
saved as well, as much of the option processing and variable state can be
shared between the utilities.

## cd

Change to a new directory.  

* `cd dir`: Change to a specific directory called `dir`.
* `cd`: Change to the HOME directory, `/root`.
* `cd -`: Change back to the previous directory.

Examples:

    /root# cd /usr
    /usr# cd /mnt/sd
    /mnt/sd# cd -
    /mnt/sd# pwd
    /mnt/sd
    /usr# cd
    /root# _

## ls

TODO

## pwd

Prints the current working directory.

## uname

Prints identification information for the operating system and machine.
Options:

* `-a`: Print all identification strings.
* `-s`: Print the system name ("MOSnix").
* `-n`: Print the node name ("mosnix").
* `-r`: Print the kernel release number; same as `-v`.
* `-v`: Print the kernel version.
* `-m`: Print the machine architecture, such as `mos6502` or `mos65c02`.

Without options, the behaviour is the same as `uname -s`.
