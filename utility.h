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
#include "command.h"


static char *getPath(){
    static char path[MAXPATH];
    getcwd(path, MAXPATH);
    return path;
}

int setpath(char *newPath);

int addPath(char *newPath);

void err_sys(char *msg, int outFd);

bool isEmpty(char *str, size_t strLength);

ssize_t isPipe(char *cmd, size_t cmdLength);

bool isBackground(char *cmd);

bool isInternalCmd(char *cmd, size_t cmdLength);

int isIoRedirect(char *cmd, size_t cmdLength);



#endif //MYSHELL_UTILITY_H
