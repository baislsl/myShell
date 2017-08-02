//
// Created by baislsl on 17-7-22.
//

#ifndef MYSHELL_INTERNAL_H
#define MYSHELL_INTERNAL_H

#include <zconf.h>
#include <stdio.h>
#include "utility.h"

int execInner(char *name, const char **argv, size_t argc);
bool isInternalCmd(char *name, size_t length);

#endif //MYSHELL_INTERNAL_H
