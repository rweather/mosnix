#!/usr/bin/python
#
# Generate the client side declarations in <mosnix/unistd.h>.

import gentools
import sys
import re

file = open(sys.argv[1], 'r')
lines = file.readlines()
file.close()

gentools.print_header("", cplusplus=False, include=['<mosnix/syscall.h>'])

print("/* Generated automatically */")
print("")
print("void * const SYSCALL[128] = {")

next_syscall = 0

for line in lines:
    if line.startswith('#'):
        continue
    fields = line.strip().split('|')
    fields = [s.strip() for s in fields]
    name = fields[1].replace('_','')
    number = int(fields[0])
    while next_syscall < number:
        print("    /* %3d */ (void *)sys_notimp," % next_syscall)
        next_syscall = next_syscall + 1
    print("    /* %3d */ (void *)sys_%s," % (number, name))
    next_syscall = next_syscall + 1

while next_syscall < 127:
    print("    /* %3d */ (void *)sys_notimp," % next_syscall)
    next_syscall = next_syscall + 1
print("    /* %3d */ (void *)sys_notimp" % next_syscall)

print("};")

gentools.print_footer(cplusplus=False, endif=False)
