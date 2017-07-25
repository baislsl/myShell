//
// Created by baislsl on 17-7-22.
//

#ifndef MYSHELL_INTERNAL_H
#define MYSHELL_INTERNAL_H

#include <zconf.h>
#include <stdio.h>
#include "utility.h"

typedef int innerFunc(const char **argv, size_t n);

int execInner(char *name, const char **argv, size_t argc);

#endif //MYSHELL_INTERNAL_H
