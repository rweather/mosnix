#!/usr/bin/python
#
# Generate the system call number list for <bits/syscall.h>.

import gentools
import sys
import re

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
    name = re.sub(r'^_', '', fields[1])
    name = name.replace('%', '')
    print("#define SYS_%s %s" % (name, fields[0]))

gentools.print_footer(cplusplus=False)
