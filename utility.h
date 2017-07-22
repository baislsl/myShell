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
typedef bool Func(char cc);

void err_sys(char *msg, int outFd);
int findCharacter(char *str, size_t strLength, size_t from[], size_t to[], size_t indexSize, Func *func);
bool isSpace(char cc);
ssize_t ridFind(char *str, size_t strLength, char *store[], size_t storeSize, Func *func);
ssize_t spaceSplit(char *str, size_t strLength, char *store[], size_t storeSize);

#endif //MYSHELL_UTILITY_H
