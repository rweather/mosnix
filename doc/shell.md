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
    /mnt/sd# pwd
    /mnt/sd
    /mnt/sd# cd -
    /usr# cd
    /root# _

## ls

Lists the contents of a directory.

* `ls`: List the contents of the current directory.
* `ls dir`: List the contents of a specified directory.
* `ls dir1 dir2`: List the contents of multiple directories.

The following options are currently supported:

* `-a`: Show hidden files that begin with `.` as well as normal files.
* `-l`: Show the file details in long format with mode and size.
* `-F`: Show the file or directory type with `*` and `/' after the filename.

## mount

Lists all active mount points.  It not possible to mount or unmount
other directories at the moment.

## pwd

Prints the current working directory.

## uname

Prints identification information for the operating system and machine.
Options:

* `-a`: Print all identification strings.
* `-s`: Print the system name ("MOSnix").
* `-n`: Print the node name ("mosnix").
* `-r`: Print the kernel release number.
* `-v`: Print the version of the hardware the kernel is running on.
* `-m`: Print the machine architecture, such as `mos6502` or `mos65c02`.

Without options, the behaviour is the same as `uname -s`.
