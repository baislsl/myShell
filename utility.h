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

void err_sys(char *msg, int outFd);

ssize_t isPipe(char *cmd, size_t cmdLength);

bool isBackground(char *cmd);

bool isInternalCmd(char *cmd, size_t cmdLength);

int isIoRedirect(char *cmd, size_t cmdLength);

int isOutputRedirect(char *str, size_t strLength);

int isInputRedirect(char *str, size_t strLength);





#endif //MYSHELL_UTILITY_H
