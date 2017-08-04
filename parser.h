//
// Created by baislsl on 17-7-23.
//

#ifndef MYSHELL_PARSER_H
#define MYSHELL_PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <zconf.h>

typedef bool Func(char cc);

int findCharacter(char *str, size_t strLength, size_t index[], size_t indexSize, Func *func);

bool isSpace(char cc);

ssize_t spaceSplit(char *str, size_t strLength, char *store[], size_t storeSize);

ssize_t pipeSplit(char *str, size_t strLength, char *store[], size_t storeSize);

#endif //MYSHELL_PARSER_H
