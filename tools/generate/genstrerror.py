#!/usr/bin/python
#
# Generate the error string list for strerror().

import gentools
import sys

file = open(sys.argv[1], 'r')
lines = file.readlines()
file.close()

gentools.print_header("", cplusplus=False, include=["<errno.h>", "<mosnix/syscall.h>"])

print("/* Generated automatically */")
print("")
print("static const char * const errlist[] = {")
print('    "No error",')

for line in lines:
    if line.startswith('#'):
        continue
    fields = line.strip().split('|')
    fields = [s.strip() for s in fields]
    if fields[1].startswith('E'):
        continue
    print('    "%s",' % fields[2])

print('};')
print("#define errno_count ((int)(sizeof(errlist) / sizeof(errlist[0])))")
print("")

print("int sys_strerror(struct sys_strerror_s *args)")
print("{")
print("    int errnum = args->errnum;")
print("    if (errnum >= 0 && errnum < errno_count)")
print("        *(args->result) = (char *)errlist[errnum];")
print("    else")
print('        *(args->result) = (char *)"Error";')
print("    return 0;")
print("}")

gentools.print_footer(cplusplus=False, endif=False)
