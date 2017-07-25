//
// Created by baislsl on 17-7-23.
//

#include <wait.h>
#include <fcntl.h>
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

int setDirect(CommandPtr cmd) {
    if (!cmd->inFlag) {
        // close(cmd->inWritePipe);
    }
    cmd->inWritePipe = dup(STDIN_FILENO);
    dup2(cmd->inFd, STDIN_FILENO);

    if (!cmd->outFlag) {
        // close(cmd->outReadPipe);
    }
    cmd->outReadPipe = dup(STDOUT_FILENO);
    dup2(cmd->outFd, STDOUT_FILENO);
}

int resetDirect(CommandPtr cmd) {
    if (cmd->inFd != STDIN_FILENO) {
        close(cmd->inFd);
    }
    dup2(cmd->inWritePipe, STDIN_FILENO);
    close(cmd->inWritePipe);
    if (cmd->outFd != STDOUT_FILENO) {
        close(cmd->outFd);
    }
    dup2(cmd->outReadPipe, STDOUT_FILENO);
    close(cmd->outReadPipe);
}


int setInDirect(CommandPtr cmd, bool flag, int inFd, int inWritePipe) {
    cmd->inFlag = flag;
    cmd->inFd = inFd;
    if (!flag) {
        cmd->inWritePipe = inWritePipe;
    }
}

int setOutDirect(CommandPtr cmd, bool flag, int outFd, int outReadPipe) {
    cmd->outFlag = flag;
    cmd->outFd = outFd;
    if (!flag) {
        cmd->outReadPipe = outReadPipe;
    }
}

int buildCmd(CommandPtr ptr, char *cmd, size_t cmdLength) {
    ptr->inFd = STDIN_FILENO;   // set default in
    ptr->outFd = STDOUT_FILENO; // set default out
    ptr->inFlag = ptr->outFlag = false;
    char args_data[MAXLINE / 2 + 1][MAXLENGTH];
    char *args[MAXLINE / 2 + 1];
    for (int i = 0; i < MAXLINE / 2 + 1; i++) {
        args[i] = args_data[i];
    }
    ssize_t n = spaceSplit(cmd, cmdLength, args, MAXLINE / 2 + 1);
    if (n < 0)
        return -1;
    ptr->argc = (size_t) n;
    for (size_t i = 0; i < ptr->argc; i++) {
        ptr->argv[i] = (char *) malloc(strlen(args[i]) + 1);
        strcpy(ptr->argv[i], args[i]);
    }
    ptr->argv[ptr->argc] = NULL;    // 将最后一个参数后的参数指针设为NULL, 防止执行外部命令时误读入多余的参数
    return 1;
}

char *commandName(CommandPtr cmd) {
    return cmd->argv[0];
}

int runOuterCmd(CommandPtr cmd) {
    int status;
    int pid = fork();
    if (pid < 0) {
        err_sys("fork error", STDOUT_FILENO);
    } else if (pid == 0) {
        if (cmd->argc < 0) {
            err_sys("to many arguments", cmd->outFd);
            exit(1);
        } else {
            execvp(cmd->argv[0], cmd->argv);
        }
    } else {
        if (wait(&status) != pid) {
            err_sys("wait error", STDOUT_FILENO);
            exit(1);
        }
    }
}

int runInternalCmd(CommandPtr cmd) {
    char *cmdName = commandName(cmd);
    return execInner(cmdName, cmd->argv, cmd->argc);
}

int execCommand(CommandPtr cmd) {
    char *cmdName = commandName(cmd);
    setDirect(cmd);
    if (isInternalCmd(cmdName, strlen(cmdName))) {
        runInternalCmd(cmd);
    } else {
        runOuterCmd(cmd);
    }
    resetDirect(cmd);
}

void freeCommand(CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }
}


int getInputRedirect(const CommandPtr cmd) {
    int fd = 0;
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp("<", cmd->argv[i]) == 0) {
            if (i == cmd->argc - 1)  // no direct file given
                return 0;
            fd = open(cmd->argv[i + 1], O_RDONLY);
            if (fd < 0)
                return 0;
            else
                return fd;

        }
    }
    return 0;
}

int getOutputRedirect(const CommandPtr cmd) {
    int fd = 0;
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp(">", cmd->argv[i]) == 0) {
            if (i == cmd->argc - 1)  // no direct file given
                return 0;
            fd = open(cmd->argv[i + 1], O_CREAT | O_WRONLY | O_TRUNC);
            break;
        }
        if (strcmp(">>", cmd->argv[i]) == 0) {
            if (i == cmd->argc - 1)  // no direct file given
                return 0;
            fd = open(cmd->argv[i + 1], O_CREAT | O_WRONLY | O_APPEND);
            break;
        }
    }
    if (fd < 0)
        return 0;
    else
        return fd;
}

int isInputRedirect(const CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp("<", cmd->argv[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int isOutputRedirect(const CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp(">", cmd->argv[i]) == 0) {
            return 1;
        } else if (strcmp(">>", cmd->argv[i]) == 0) {
            return 2;
        }
    }
    return 0;
}

int ridIO(CommandPtr cmd, char *str){
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp(str, cmd->argv[i]) == 0) {
            if(i == cmd->argc - 1){
                cmd->argc -= 1;
                cmd->argv[i] = NULL;
                return 0;
            }

            for(size_t j = i + 2; j < cmd->argc; j++, i++){
                strcpy(cmd->argv[i], cmd->argv[j]);
            }
            cmd->argc -= 2;
            cmd->argv[i] = NULL;
            return 0;
        }
    }
    return 0;
}

int ridInputRedirect(CommandPtr cmd){
    ridIO(cmd, "<");
}

int ridOutputRedirect(CommandPtr cmd){
    ridIO(cmd, ">");
    ridIO(cmd, ">>");
}
