
This directory contains the skeleton for the root filesystem under MOSnix.
The regular files in this directory tree contain rules to turn them into
special devices, symbolic links, or mount points:

* `char,m,n` - character special device, major=m, minor=n
* `block,m,n` - block special device, major=m, minor=n
* `symlink,path` - symbolic link to another path
* `mount,/dev/x` - mount point that auto-mounts block special device `/dev/x`
* `emptydir` - directory exists but is empty by default
* `emptydir,private` - empty directory with private drwx------ permissions

At startup, the shell creates the skeleton of the root filesystem and
auto-mounts other filesystems.
