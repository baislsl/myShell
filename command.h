//
// Created by baislsl on 17-7-23.
//

#ifndef MYSHELL_COMMAND_H
#define MYSHELL_COMMAND_H

#include <glob.h>

typedef struct _Command Command;
typedef struct _Command* CommandPtr;

struct _Command {
    size_t argc;
    char *argv[256];
    int inFd;
    int outFd;
};

int buildCmd(CommandPtr ptr, char *cmd, int cmdLength, int inFd, int outFd);

int execCommand(CommandPtr cmd);

void freeCommand(CommandPtr cmd);
#endif //MYSHELL_COMMAND_H
