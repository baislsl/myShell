//
// Created by baislsl on 17-7-23.
//

#ifndef MYSHELL_COMMAND_H
#define MYSHELL_COMMAND_H

#include <glob.h>
#include "myshell.h"

typedef struct _Command Command;
typedef struct _Command* CommandPtr;

struct _Command {
    size_t argc;
    char *argv[256];
    int inFd;
    int outFd;
    int inWritePipe;
    int outReadPipe;
    bool inFlag, outFlag;   // inflag true is in put not pipe
};

int buildCmd(CommandPtr ptr, char *cmd, int cmdLength);

int execCommand(CommandPtr cmd);

void freeCommand(CommandPtr cmd);

int setInDirect(CommandPtr cmd, bool flag, int inFd, int inWritePipe);

int setOutDirect(CommandPtr cmd, bool flag, int outFd, int outReadPipe);
#endif //MYSHELL_COMMAND_H
