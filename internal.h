//
// Created by baislsl on 17-7-22.
//

#ifndef MYSHELL_INTERNAL_H
#define MYSHELL_INTERNAL_H

#include <zconf.h>
#include <stdio.h>
#include <stdbool.h>

int execInner(char *name, const char **argv, size_t argc);
bool isInternalCmd(char *name, size_t length);


typedef int innerFunc(const char **argv, size_t n);

struct tie {
    innerFunc *func;
    char *cmd;
};
typedef struct tie internalFuncTie;

#endif //MYSHELL_INTERNAL_H
