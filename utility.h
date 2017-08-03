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

char *getPath();

int setpath(char *newPath);

int addPath(char *newPath);

void err_sys(char *msg);

bool isEmpty(char *str, size_t strLength);

void runCommand(char *cmd);

ssize_t readCommand(char *cmd);

void printInfo();

#endif //MYSHELL_UTILITY_H
