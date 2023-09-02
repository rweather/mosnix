#!/usr/bin/python
#
# Generate the client side library stubs in unistd.c.

import gentools
import sys
import re

file = open(sys.argv[1], 'r')
lines = file.readlines()
file.close()

gentools.print_header("", cplusplus=False, include=["<unistd.h>", "<sys/syscall.h>"])

print("/* Generated automatically */")
print("")

for line in lines:
    if line.startswith('#'):
        continue
    fields = line.strip().split('|')
    fields = [s.strip() for s in fields]
    name = fields[1]
    returnType = fields[2]
    if len(fields) <= 3 or fields[3] == 'void':
        print("%s %s(void)" % (returnType, name))
        print("{")
        if returnType != 'void':
            print("    return syscall(SYS_%s);" % name)
        else:
            print("    syscall(SYS_%s);" % name)
            print("    while (1) ; /* stop compiler complaining about reachable unreachable code */")
        print("}")
    else:
        returnType = fields[2]
        returnName = ''
        last = fields[len(fields)-1]
        if last.startswith('>'):
            last = last[1:]
            args = fields[3:len(fields)-1]
            returnType = last.split(' ')[0]
            returnName = last.split('*')[1]
        else:
            args = fields[3:]
        argNames = []
        for arg in args:
            argNames.append(re.findall(r'[A-Za-z0-9_]+$', arg)[0])
        print("%s %s(%s)" % (returnType, name, ", ".join(args)))
        print("{")
        if len(returnName) > 0:
            print("    %s %s;" % (returnType, returnName))
            print("    if (syscall(SYS_%s, %s, &%s) == 0)" % (re.sub(r'^_', '', name), ", ".join(argNames), returnName))
            print("        return %s;" % returnName)
            print("    else")
            print("        return -1;")
        elif returnType == 'void':
            print("    syscall(SYS_%s, %s);" % (re.sub(r'^_', '', name), ", ".join(argNames)))
            print("    while (1) ; /* stop compiler complaining about reachable unreachable code */")
        else:
            print("    return syscall(SYS_%s, %s);" % (re.sub(r'^_', '', name), ", ".join(argNames)))
        print("}")
    print("")

gentools.print_footer(cplusplus=False, endif=False)
