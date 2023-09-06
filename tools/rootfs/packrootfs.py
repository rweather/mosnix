#!/usr/bin/python
#
# Packs the rootfs filesystem into a set of instructions for the shell
# to construct the root filesystem skeleton at runtime.

import sys
import os
import pprint

# Mode bits.
S_IFMT      = 0xF000
S_IFIFO     = 0x1000
S_IFCHR     = 0x2000
S_IFDIR     = 0x4000
S_IFBLK     = 0x6000
S_IFREG     = 0x8000
S_IFLNK     = 0xA000
S_IFSOCK    = 0xC000
S_ISUID     = 0x0800
S_ISGID     = 0x0400
S_ISVTX     = 0x0200
S_IRWXU     = 0x01C0
S_IRUSR     = 0x0100
S_IWUSR     = 0x0080
S_IXUSR     = 0x0040
S_IRWXG     = 0x0038
S_IRGRP     = 0x0020
S_IWGRP     = 0x0010
S_IXGRP     = 0x0008
S_IRWXO     = 0x0007
S_IROTH     = 0x0004
S_IWOTH     = 0x0002
S_IXOTH     = 0x0001

# Directory types.
DIR_NONE    = 0
DIR_REGULAR = 1
DIR_CURRENT = 2
DIR_PARENT  = 3
DIR_MOUNT   = 4

# Packed filesystem that is being built.
packed = []

# Parse a device number.
def parse_device_number(s):
    s2 = s.split(',')
    return int(s2[0]) * 256 + int(s2[1])

# Walk a directory and collect up all files and directories within it.
def walk(abspath, relpath):
    global packed
    nodes = []
    nodes.append({'name': '.', 'props': {
        'mode': S_IRUSR | S_IRGRP | S_IROTH | S_IXUSR | S_IXGRP | S_IXOTH | S_IFDIR,
        'dirtype': DIR_CURRENT,
        'devnum': 0,
        'content': []
    }})
    nodes.append({'name': '..', 'props': {
        'mode': S_IRUSR | S_IRGRP | S_IROTH | S_IXUSR | S_IXGRP | S_IXOTH | S_IFDIR,
        'dirtype': DIR_PARENT,
        'devnum': 0,
        'content': []
    }})
    for name in sorted(os.listdir(abspath)):
        if name == "README.md":
            continue
        newabs = abspath + '/' + name
        newrel = relpath + '/' + name
        mode = S_IRUSR | S_IRGRP | S_IROTH
        dirtype = DIR_NONE
        devnum = 0
        content = ''
        if os.path.isdir(newabs):
            # Descend into the subdirectory.
            mode |= S_IXUSR | S_IXGRP | S_IXOTH | S_IFDIR
            mode |= S_IWUSR | S_IWGRP
            dirtype = DIR_REGULAR
            content = walk(newabs, newrel)
        else:
            first = ''
            with open(newabs) as file:
                first = file.readline().strip()
            if first.startswith('char,'):
                # Character special device.
                mode |= S_IWUSR | S_IWGRP | S_IFCHR
                mode &= ~S_IROTH
                devnum = parse_device_number(first[5:])
            elif first.startswith('block,'):
                # Block special device.
                mode |= S_IWUSR | S_IWGRP | S_IFBLK
                mode &= ~S_IROTH
                devnum = parse_device_number(first[6:])
            elif first.startswith('symlink,'):
                # Symbolic link.
                mode |= S_IFLNK
                content = first[8:]
            elif first.startswith('mount,'):
                # Directory that can be mounted on.  We use the VTX
                # bit to indicate a valid mount point.
                mode |= S_IXUSR | S_IXGRP | S_IXOTH | S_IFDIR
                mode |= S_IWUSR | S_IWGRP
                mode |= S_ISVTX
                dirtype = DIR_MOUNT
                content = first[6:]
                if content == 'any':
                    # Anything can be mounted here.  Otherwise the content
                    # is the name of the block device to auto-mount.
                    content = ''
            elif first.startswith('emptydir'):
                # Empty directory.
                if first == "emptydir,private":
                    # /root directory needs restricted permissions.
                    mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IFDIR
                else:
                    mode |= S_IXUSR | S_IXGRP | S_IXOTH | S_IFDIR
                    mode |= S_IWUSR | S_IWGRP
                dirtype = DIR_REGULAR
                content = ''
            elif os.access(abspath, os.X_OK):
                # Regular file that is executable.
                mode |= S_IXUSR | S_IXGRP | S_IXOTH | S_IFREG
                mode |= S_IWUSR | S_IWGRP
                with open(newabs, 'rb') as file:
                    content = file.read()
            else:
                # Regular file that is not executable.
                mode |= S_IWUSR | S_IWGRP | S_IFREG
                with open(newabs, 'rb') as file:
                    content = file.read()
        nodes.append({'name': name, 'props': {
            'mode': mode,
            'dirtype': dirtype,
            'devnum': devnum,
            'content': content
        }})
    return nodes

tree = walk(sys.argv[1], '')

# TODO
pp = pprint.PrettyPrinter(depth=10)
pp.pprint(tree)
