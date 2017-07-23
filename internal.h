//
// Created by baislsl on 17-7-22.
//

#ifndef MYSHELL_INTERNAL_H
#define MYSHELL_INTERNAL_H

#include <zconf.h>
#include <stdio.h>
#include "utility.h"

int pwd(char *args[], size_t n);

int cd(char *args[], const size_t n);

static char *getPath(){
    static char path[MAXPATH];
    getcwd(path, MAXPATH);
    return path;
}
#endif //MYSHELL_INTERNAL_H
