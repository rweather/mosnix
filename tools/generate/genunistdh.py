#!/usr/bin/python
#
# Generate the client side declarations in <mosnix/unistd.h>.

import gentools
import sys
import re

file = open(sys.argv[1], 'r')
lines = file.readlines()
file.close()

gentools.print_header("MOSNIX_BITS_UNISTD_H", cplusplus=True, include=['<sys/types.h>'])

print("/* Generated automatically */")
print("")

for line in lines:
    if line.startswith('#'):
        continue
    fields = line.strip().split('|')
    fields = [s.strip() for s in fields]
    name = fields[1]
    if name.endswith('%'):
        # Don't declare this name in <unistd.h>.
        continue
    returnType = fields[2]
    if len(fields) <= 3 or fields[3] == 'void':
        print("extern %s %s(void);" % (returnType, name))
    else:
        returnType = fields[2]
        last = fields[len(fields)-1]
        if last.startswith('>'):
            last = last[1:]
            args = fields[3:len(fields)-1]
            returnType = last.split(' ')[0]
        else:
            args = fields[3:]
        print("extern %s %s(%s);" % (returnType, name, ", ".join(args)))

gentools.print_footer(cplusplus=True, endif=True)
