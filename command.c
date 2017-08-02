//
// Created by baislsl on 17-7-23.
//

#include <wait.h>
#include <fcntl.h>
#include "command.h"
#include "parser.h"
#include "internal.h"
#include "pid.h"


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

int commandCopy(CommandPtr dest, CommandPtr src){
    dest->argc = src->argc;
    dest->outFd = src->outFd;
    dest->inFd = src->inFd;
    dest->oldInFd = src->oldInFd;
    dest->oldOutFd = src->oldOutFd;
    for(size_t i =0;i<src->argc;++i){
        dest->argv[i] = (char*)malloc(strlen(src->argv[i]) + 1);
        strcpy(dest->argv[i], src->argv[i]);
    }

}

int setDirect(CommandPtr cmd) {
    cmd->oldInFd = dup(STDIN_FILENO);
    dup2(cmd->inFd, STDIN_FILENO);

    cmd->oldOutFd = dup(STDOUT_FILENO);
    dup2(cmd->outFd, STDOUT_FILENO);
}

int resetDirect(CommandPtr cmd) {
    if (cmd->inFd != STDIN_FILENO) {
        close(cmd->inFd);
    }
    dup2(cmd->oldInFd, STDIN_FILENO);
    close(cmd->oldInFd);
    if (cmd->outFd != STDOUT_FILENO) {
        close(cmd->outFd);
    }
    dup2(cmd->oldOutFd, STDOUT_FILENO);
    close(cmd->oldOutFd);
}


int setInDirect(CommandPtr cmd, bool flag, int inFd, int inWritePipe) {
    cmd->inFd = inFd;
    if (!flag) {
        cmd->oldInFd = inWritePipe;
    }
}

int setOutDirect(CommandPtr cmd, bool flag, int outFd, int outReadPipe) {
    cmd->outFd = outFd;
    if (!flag) {
        cmd->oldOutFd = outReadPipe;
    }
}

int buildCmd(CommandPtr ptr, char *cmd, size_t cmdLength) {
    ptr->inFd = STDIN_FILENO;   // set default in
    ptr->outFd = STDOUT_FILENO; // set default out
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

void handleBackExit2(int _pid) {
    pid_t pid = (pid_t) getpid();
    int number = getPidNumber(pid);
    fprintf(stdout, "[%d]:%d\n", number, pid);
    removePid(pid);
}


extern int forePid;

int runInternalCmd(CommandPtr cmd) {
    char *cmdName = commandName(cmd);
    return execInner(cmdName, (const char **) cmd->argv, cmd->argc);
}

// !!!!!!!!!!!!!!!!!!111
int execCommand(CommandPtr cmd) {
    int ret = 0;
    char *cmdName = commandName(cmd);
    bool internal = isInternalCmd(cmdName, strlen(cmdName));
    bool backgoround = isBackground(cmd);
    testAll();
    if (backgoround) {
        ridBackgroundChar(cmd);
    }
    setDirect(cmd);
    if (internal & !backgoround) {
        ret = runInternalCmd(cmd);
    } else {
        pid_t pid;
        int status;
        if ((pid = fork()) < 0) {
            err_sys("error to create a new process", STDERR_FILENO);
        } else if (pid == 0) { // child
            if (internal) {
                runInternalCmd(cmd);
            } else {
                execvp(cmd->argv[0], cmd->argv);
            }
        } else {  //parent
            if (backgoround) {
                addPid(pid, cmd, BACKGROUND);
            } else {
                forePid = pid;
                fprintf(stdout, "set forepid to %d\n ", pid);
                if (waitpid(pid, &status, WUNTRACED) != pid) {
                    err_sys("wait pid error", STDERR_FILENO);
                }
                if(WIFSTOPPED(status)){
                    addPid(pid, cmd, STOP);
                }
                fprintf(stdout, "reset forepid to -1\n ");
                forePid = -1;
            }
        }
    }
    resetDirect(cmd);
    return ret;
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

int ridIO(CommandPtr cmd, char *str) {
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp(str, cmd->argv[i]) == 0) {
            free(cmd->argv[i]);
            if (i == cmd->argc - 1) {
                cmd->argc -= 1;
                cmd->argv[i] = NULL;
                return 0;
            }

            // = to set last NULL
            for (size_t j = i + 2; j <= cmd->argc; j++, i++) {
                cmd->argv[i] = cmd->argv[j];
            }
            cmd->argc -= 2;
            return 0;
        }
    }
    return 0;
}

int ridInputRedirect(CommandPtr cmd) {
    ridIO(cmd, "<");
}

int ridOutputRedirect(CommandPtr cmd) {
    ridIO(cmd, ">");
    ridIO(cmd, ">>");
}

void ridBackgroundChar(CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp("&", cmd->argv[i]) == 0) {
            free(cmd->argv[i]);
            for (; i < cmd->argc; i++) {
                cmd->argv[i] = cmd->argv[i + 1];
            }
            --cmd->argc;
            return;
        }
    }
}

bool isBackground(const CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp("&", cmd->argv[i]) == 0)
            return true;
    }
    return false;
}

void printCommand(CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        fputs(cmd->argv[i], stdout);
        if (i != cmd->argc - 1)
            fputc(' ', stdout);
        else if(i != 0)
            fputc('\n', stdout);
    }
}