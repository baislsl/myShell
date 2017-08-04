//
// Created by baislsl on 17-8-3.
//

#include <stddef.h>

#ifndef MYSHELL_PARAM_H
#define MYSHELL_PARAM_H

int getArgc();

void paramInit(int argc, char *argv[]);

void setExitState(int state);

int getExitState();

const char *param(size_t index);

int shift(size_t i);

void printAllParam(char *dest);

#endif //MYSHELL_PARAM_H
