
The root filesystem is implemented in RAM. with provision to mount other
filesystems at various points.  At startup, the shell creates the directories
and files of the root filesystem and auto-mounts other filesystems like the
SD card interface.

Each element in the RAM filesystem is 18 bytes in size.  The uniform size
helps with memory allocation and reclamation of deleted blocks.

    struct ramfs_inode
    {
        uint16_t nlink;                 // Number of links to the inode.
        uint16_t mode;                  // Type and permission bits.
        uint32_t mtime;                 // Modification time.
        uint32_t ctime;                 // Creation time.
        uint16_t size;                  // Size of the file or symlink data.
        uint16_t mount;                 // Mount point number, or zero.
        union {
            struct ramfs_data *data;    // File or symbolic link data.
            struct ramfs_dirent *dir;   // Sub-directory data.
            dev_t device;               // Device type.
        };
    };

Data for regular files and symbolic links is stored as a linked list of
data blocks:

    struct ramfs_data
    {
        uint8_t data[16];
        struct ramfs_data *next;
    };

The low bits of the `size` field in the inode indicates how much of the
final data block is currently in use.

Data for directories is stored as a linked list of directory entries:

    struct ramfs_dirent
    {
        char name[14];
        struct ramfs_inode *inode;
        struct ramfs_dirent *next;
    };
