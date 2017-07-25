//
// Created by baislsl on 17-7-22.
//

#ifndef MYSHELL_INTERNAL_H
#define MYSHELL_INTERNAL_H

#include <zconf.h>
#include <stdio.h>
#include "utility.h"

typedef int innerFunc(const char *args[], size_t n);

int pwd(const char *args[], size_t n);

int cd(const char *args[],  size_t n);

int clr(const char *args[], size_t n);

static char *getPath(){
    static char path[MAXPATH];
    getcwd(path, MAXPATH);
    return path;
}
#endif //MYSHELL_INTERNAL_H
