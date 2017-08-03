//
// Created by baislsl on 17-7-23.
//

#ifndef MYSHELL_COMMAND_H
#define MYSHELL_COMMAND_H

#include <glob.h>
#include <stdbool.h>

typedef struct _Command Command;
typedef struct _Command *CommandPtr;

struct _Command {
    size_t argc;
    char *argv[256];
    int inFd;
    int outFd;
    int oldInFd;
    int oldOutFd;
};

int commandCopy(CommandPtr src, CommandPtr dest);

int buildCmd(CommandPtr ptr, char *cmd, size_t cmdLength);

int execCommand(CommandPtr cmd);

void freeCommand(CommandPtr cmd);

int setInDirect(CommandPtr cmd, bool flag, int inFd, int inWritePipe);

int setOutDirect(CommandPtr cmd, bool flag, int outFd, int outReadPipe);


int isInputRedirect(CommandPtr cmd);

int isOutputRedirect(CommandPtr cmd);

int getOutputRedirect(CommandPtr cmd);

int getInputRedirect(CommandPtr cmd);

int ridInputRedirect(CommandPtr cmd);

int ridOutputRedirect(CommandPtr cmd);

void ridBackgroundChar(CommandPtr cmd);

void printCommand(CommandPtr cmd);

bool isBackground(CommandPtr cmd);

#endif //MYSHELL_COMMAND_H
