
copyright = """/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */"""

def print_header(protect, cplusplus=True, include=[]):
    print(copyright)
    if len(protect) > 0:
        print("")
        print("#ifndef %s" % protect)
        print("#define %s" % protect)
    if len(include) > 0:
        print("")
        for i in include:
            print("#include %s" % i)
    if cplusplus:
        print("")
        print("#ifdef __cplusplus")
        print('extern "C" {')
        print("#endif")
    print("")

def print_footer(cplusplus=True, endif=True):
    if cplusplus:
        print("")
        print("#ifdef __cplusplus")
        print('}')
        print("#endif")
    if endif:
        print("")
        print("#endif")
