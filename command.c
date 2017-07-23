//
// Created by baislsl on 17-7-23.
//

#include <wait.h>
#include "command.h"
#include "myshell.h"
#include "parser.h"
#include "internal.h"


/**
* After reading user input, the steps are:
*内部命令:
*.....
*外部命令:
* (1) fork a child process using fork()
* (2) the child process will invoke execvp()
* (3) if command included &, parent will invoke wait()
*.....
*/

int buildCmd(CommandPtr ptr, char *cmd, int cmdLength, int inFd, int outFd) {
    ptr->inFd = inFd;
    ptr->outFd = outFd;
    char args_data[MAXLINE / 2 + 1][MAXLENGTH];
    char *args[MAXLINE / 2 + 1];
    for (int i = 0; i < MAXLINE / 2 + 1; i++) {
        args[i] = args_data[i];
    }
    ssize_t n = spaceSplit(cmd, (size_t) cmdLength, args, MAXLINE / 2 + 1);
    if (n < 0)
        return -1;
    ptr->argc = (size_t) n;
    for (size_t i = 0; i < ptr->argc; i++) {
        ptr->argv[i] = (char *) malloc(strlen(args[i]));
        strcpy(ptr->argv[i], args[i]);
    }
    return 1;
}

char *commandName(CommandPtr cmd) {
    return cmd->argv[0];
}

int runOuterCmd(CommandPtr cmd) {

    int pid = fork();
    if (pid < 0) {
        err_sys("fork error", STDOUT_FILENO);
    } else if (pid == 0) {
        dup2(cmd->inFd, STDIN_FILENO);
        dup2(cmd->outFd, STDOUT_FILENO);
        if (cmd->argc < 0) {
            err_sys("to many arguments", cmd->outFd);
        } else {
            execvp(cmd->argv[0], cmd->argv);
        }
        close(cmd->inFd);
        close(cmd->outFd);
    } else {

    }


}

int runInternalCmd(CommandPtr cmd) {
    // dup2(cmd->inFd, STDIN_FILENO);
    // dup2(cmd->outFd, STDOUT_FILENO);
    char *cmdName = commandName(cmd);
    if (strcmp(cmdName, "cd") == 0) {
        cd(cmd->argv, cmd->argc);
    } else if (strcmp(cmdName, "pwd") == 0) {
        pwd(cmd->argv, cmd->argc);
    }
    // if(cmd->inFd != STDIN_FILENO)
       // close(cmd->inFd);
    // if(cmd->outFd != STDOUT_FILENO)
        // close(cmd->outFd);
    return 0;
}

int execCommand(CommandPtr cmd) {
    char *cmdName = commandName(cmd);
    if (isInternalCmd(cmdName, strlen(cmdName))) {
        runInternalCmd(cmd);
    } else {
        runOuterCmd(cmd);
    }
}

void freeCommand(CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }
}

