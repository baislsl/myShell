//
// Created by baislsl on 17-7-22.
//

#ifndef MYSHELL_UTILITY_H
#define MYSHELL_UTILITY_H

#include <glob.h>
#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>
#include "myshell.h"

static char *getPath() {
    static char path[MAX_PATH];
    getcwd(path, MAX_PATH);
    return path;
}

int setpath(char *newPath);

int addPath(char *newPath);

void err_sys(char *msg);

bool isEmpty(char *str, size_t strLength);

ssize_t isPipe(char *cmd, size_t cmdLength);

void runCommand(char *cmd);

int isIoRedirect(char *cmd, size_t cmdLength);


#endif //MYSHELL_UTILITY_H
