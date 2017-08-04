//
// Created by baislsl on 17-7-23.
//

#include <wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "parser.h"
#include "internal.h"
#include "pid.h"
#include "forePid.h"
#include "myshell.h"
#include "utility.h"

// copy a command
void commandCopy(CommandPtr dest, CommandPtr src) {
    dest->argc = src->argc;
    dest->outFd = src->outFd;
    dest->inFd = src->inFd;
    dest->oldInFd = src->oldInFd;
    dest->oldOutFd = src->oldOutFd;
    for (size_t i = 0; i < src->argc; ++i) {
        dest->argv[i] = (char *) malloc(strlen(src->argv[i]) + 1);
        strcpy(dest->argv[i], src->argv[i]);
    }

}

// before a command executes, use dup2 to redirect the input and output
void setDirect(CommandPtr cmd) {
    cmd->oldInFd = dup(STDIN_FILENO);
    dup2(cmd->inFd, STDIN_FILENO);

    cmd->oldOutFd = dup(STDOUT_FILENO);
    dup2(cmd->outFd, STDOUT_FILENO);
}

// after a command was executed, use dup2 to redirect the input and output
void resetDirect(CommandPtr cmd) {
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

// set input file descriptor of the command
void setInDirect(CommandPtr cmd, int inFd) {
    cmd->inFd = inFd;
}

// set output file descriptor of the command
void setOutDirect(CommandPtr cmd, int outFd) {
    cmd->outFd = outFd;
}

/**
 * build a command from a string
 * the string must not contain pipe '|', but can contain redirect (>, <, >>),
 *
 * @return return 0 if build successfully, return -1 if fail
 * */
int buildCmd(CommandPtr ptr, char *cmd, size_t cmdLength) {
    ptr->inFd = STDIN_FILENO;   // set default in
    ptr->outFd = STDOUT_FILENO; // set default out
    char args_data[MAX_LINE / 2 + 1][MAX_LENGTH];
    char *args[MAX_LINE / 2 + 1];
    for (int i = 0; i < MAX_LINE / 2 + 1; i++) {
        args[i] = args_data[i];
    }

    // split the string by blank character
    ssize_t n = spaceSplit(cmd, cmdLength, args, MAX_LINE / 2 + 1);
    if (n < 0)
        return -1;
    ptr->argc = (size_t) n;
    for (size_t i = 0; i < ptr->argc; i++) {
        ptr->argv[i] = (char *) malloc(strlen(args[i]) + 1);
        strcpy(ptr->argv[i], args[i]);
    }

    // set last argv to NULL, mark as the end of the argument
    // this is necessary when we use execvp to conduct the command
    ptr->argv[ptr->argc] = NULL;
    return 0;
}

// get the command name of a command
char *commandName(CommandPtr cmd) {
    return cmd->argv[0];
}

int runInternalCmd(CommandPtr cmd) {
    return execInternal((const char **) cmd->argv, cmd->argc);
}

int runOuterCmd(CommandPtr cmd) {
    execvp(cmd->argv[0], cmd->argv);
}

int execCommand(CommandPtr cmd) {
    int ret = 0;
    char *cmdName = commandName(cmd);
    bool internal = isInternalCmd(cmdName, strlen(cmdName));
    bool background = isBackground(cmd);

    // before each the execution of each command,
    // check the condition of each running process
    checkProcess();

    if (background) {   // rib '&' in command
        ridBackgroundChar(cmd);
    }
    setDirect(cmd);
    if (internal & !background) {   // inner, foreground
        ret = runInternalCmd(cmd);
    } else {
        pid_t pid;
        if ((pid = fork()) < 0) {
            err_sys("error to create a new process");
        } else if (pid == 0) {  // child
            if (internal) {         // inner, background
                runInternalCmd(cmd);
            } else {                // outer,  foreground/background
                runOuterCmd(cmd);
            }
        } else {  //parent
            if (background) {       // inner/outer, background
                addPid(pid, cmd, BACKGROUND);
            } else {                // outer, foreground
                // put the process in foreground by called foreGoundWait
                int status = foreGroundWait(pid);

                // check the exit state
                // if it is just stop by Ctrl + Z, add the pid to plist
                if (WIFSTOPPED(status)) {
                    addPid(pid, cmd, STOP);
                }
            }
        }
    }
    resetDirect(cmd);
    return ret;
}

// free the memory of a command that was allocated in buildCmd
void freeCommand(CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }
}

// open the input redirect file and return its file descriptor
int getInputRedirect(CommandPtr cmd) {
    int fd = 0;
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp("<", cmd->argv[i]) == 0) {
            if (i == cmd->argc - 1)  // no direct file given
                return 0;
            fd = open(cmd->argv[i + 1], O_RDONLY);
            if (fd < 0)
                return 0;
            return fd;

        }
    }
    return 0;
}

// open the output redirect file and return its file descriptor
int getOutputRedirect(CommandPtr cmd) {
    int fd = 0;
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp(">", cmd->argv[i]) == 0) {
            if (i == cmd->argc - 1)  // no direct file given
                return 0;
            fd = open(cmd->argv[i + 1], O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
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
    return fd;
}

// judge whether the command is input redirect
int isInputRedirect(CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp("<", cmd->argv[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * judge whether the command is output redirect
 *
 * @return return 0 if it is not output redirect
 *  return 1 if it is output redirect as '>'
 *  return 2 if it is output redirect as '>>'
 * */
int isOutputRedirect(CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp(">", cmd->argv[i]) == 0) {
            return 1;
        } else if (strcmp(">>", cmd->argv[i]) == 0) {
            return 2;
        }
    }
    return 0;
}

// call by function ridInputRedirect and ridOutputRedirect to drop argv of redirect
int ridIO(CommandPtr cmd, char *str) {
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp(str, cmd->argv[i]) == 0) {
            free(cmd->argv[i]);
            if (i == cmd->argc - 1) {
                cmd->argc -= 1;
                cmd->argv[i] = NULL;
                return 0;
            }

            for (size_t j = i + 2; j <= cmd->argc; j++, i++) {
                cmd->argv[i] = cmd->argv[j];
            }
            cmd->argc -= 2;
            return 0;
        }
    }
    return 0;
}

// drop argv '<' and the argv after '<'
int ridInputRedirect(CommandPtr cmd) {
    ridIO(cmd, "<");
}

// drop argv '>' / '>>' and the argv after '>' / '>>'
int ridOutputRedirect(CommandPtr cmd) {
    ridIO(cmd, ">");
    ridIO(cmd, ">>");
}

// drop argv '&'
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

// judge whether the command contain '&'
bool isBackground(CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        if (strcmp("&", cmd->argv[i]) == 0)
            return true;
    }
    return false;
}

// print the command to stdout
void printCommand(CommandPtr cmd) {
    for (size_t i = 0; i < cmd->argc; i++) {
        fputs(cmd->argv[i], stdout);
        if (i != cmd->argc - 1)
            fputc(' ', stdout);
        else
            fputc('\n', stdout);
    }
}