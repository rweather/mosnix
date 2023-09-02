#!/usr/bin/python
#
# Generate the kernel system call function declarations for <mosnix/syscall.h>.

import gentools
import sys
import re

file = open(sys.argv[1], 'r')
lines = file.readlines()
file.close()

gentools.print_header("MOSNIX_SYSCALL_H", cplusplus=True, include=["<sys/types.h>"])

print("/* Generated automatically */")
print("")

# Print the struct definitions for system call parameters.
for line in lines:
    if line.startswith('#'):
        continue
    fields = line.strip().split('|')
    fields = [s.strip() for s in fields]
    if len(fields) > 3 and fields[3] != 'void':
        print("struct sys_%s_s {" % re.sub(r'^_', '', fields[1]))
        for field in fields[3:]:
            if field.startswith(">"):
                field = field[1:]
            print("    %s;" % field)
        print("};")
        print("")

# Print the prototypes for all system call functions.
for line in lines:
    if line.startswith('#'):
        continue
    fields = line.strip().split('|')
    fields = [s.strip() for s in fields]
    name = re.sub(r'^_', '', fields[1])
    if len(fields) <= 3 or fields[3] == 'void':
        print("/* %3d */ extern int sys_%s(void);" % (int(fields[0]), name))
    elif fields[2] == 'void':
        print("/* %3d */ extern void sys_%s(struct sys_%s_s *args);" % (int(fields[0]), name, name))
    else:
        print("/* %3d */ extern int sys_%s(struct sys_%s_s *args);" % (int(fields[0]), name, name))
print("/* N/A */ extern int sys_notimp(void);")

gentools.print_footer(cplusplus=True)
