//
// Created by baislsl on 17-7-21.
//

#ifndef MYSHELL_MYSHELL_H
#define MYSHELL_MYSHELL_H

#include <wchar.h>
#include <stdbool.h>
#include <stdio.h>

typedef bool Func(char cc);

ssize_t ridFind(char *str, size_t strLength, char *store[], size_t storeSize, Func *func);
ssize_t spaceSplit(char *str, size_t strLength, char *store[], size_t storeSize);


#endif //MYSHELL_MYSHELL_H
