#!/usr/bin/python
#
# Generate the system call number list for <bits/syscall.h>.

import gentools
import sys

file = open(sys.argv[1], 'r')
lines = file.readlines()
file.close()

gentools.print_header("MOSNIX_BITS_SYSCALL_H", cplusplus=False)

print("/* Generated automatically */")
print("")

for line in lines:
    if line.startswith('#'):
        continue
    fields = line.strip().split('|')
    fields = [s.strip() for s in fields]
    print("#define SYS_%s %s" % (fields[1].replace('_',''), fields[0]))

gentools.print_footer(cplusplus=False)
