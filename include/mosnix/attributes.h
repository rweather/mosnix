/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_ATTRIBUTES_H
#define MOSNIX_ATTRIBUTES_H

#define ATTR_STRUCT_PACKED __attribute__((packed))
#define ATTR_ENUM_PACKED __attribute__((packed))
#define ATTR_SECTION_NOINIT __attribute__((section(".noinit")))
#define ATTR_SECTION_ZP __attribute__((section(".zp")))
#define ATTR_NOINLINE __attribute__((noinline))
#define ATTR_LEAF __attribute__((leaf))

#endif
