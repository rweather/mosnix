#!/usr/bin/python
#
# Generate the error number list for <bits/errno.h>.

import gentools
import sys

file = open(sys.argv[1], 'r')
lines = file.readlines()
file.close()

gentools.print_header("MOSNIX_BITS_ERRNO_H", cplusplus=False)

print("/* Generated automatically */")
print("")

for line in lines:
    if line.startswith('#'):
        continue
    fields = line.strip().split('|')
    fields = [s.strip() for s in fields]
    if len(fields) > 3:
        name = fields[3]
    else:
        name = fields[2]
    print("#define %-16s%s /* %s */" % (fields[0], fields[1], name))

gentools.print_footer(cplusplus=False)
