//
// Created by baislsl on 17-7-21.
//

#include <fcntl.h>
#include "utility.h"
#include "command.h"
#include "param.h"
#include "parser.h"

char *getPath() {
    static char path[MAX_PATH];
    getcwd(path, MAX_PATH);
    return path;
}

int setpath(char *newPath) {
    setenv("PATH", newPath, 1);
}

int addPath(char *newPath) {
    char *env = getenv("PATH");
    char path[MAX_LENGTH];
    strcpy(path, env);
    size_t len = strlen(path);
    path[len] = ':';
    path[len + 1] = 0;
    strcat(path, newPath);
    setenv("PATH", path, 1);
    return 0;
}

void err_sys(char *msg) {
    fprintf(stderr, "\033[31;1m%s\033[0ms", msg);
}

void runCommand(char *cmd) {
    if (isEmpty(cmd, strlen(cmd)))
        return;
    char spiltCmd[MAX_PIPE + 1][MAX_LENGTH];
    char *tmpSplitCmd[MAX_PIPE + 1];
    for (size_t i = 0; i < MAX_PIPE + 1; i++) {
        tmpSplitCmd[i] = spiltCmd[i];
    }
    ssize_t cmdNumbers = pipeSplit(cmd, strlen(cmd), tmpSplitCmd, MAX_PIPE + 1);

    Command command[cmdNumbers];
    int fdr, fdw;
    char tmpFile1[MAX_LINE], tmpFile2[MAX_LINE];
    tmpnam(tmpFile1);
    tmpnam(tmpFile2);
    for (size_t i = 0; i < cmdNumbers; i++) {
        if (i % 2 == 0) {
            fdw = open(tmpFile1, O_CREAT | O_WRONLY | O_TRUNC);
            fdr = open(tmpFile2, O_CREAT | O_RDONLY);
        } else {
            fdw = open(tmpFile2, O_CREAT | O_WRONLY | O_TRUNC);
            fdr = open(tmpFile1, O_CREAT | O_RDONLY);
        }
        int n = buildCmd(&command[i], spiltCmd[i], strlen(spiltCmd[i]));
        if (n < 0) {
            err_sys("build fail");
        }
        if (i == 0) {
            setInDirect(&command[i], true, STDIN_FILENO, 0);
        } else {
            setInDirect(&command[i], true, fdr, 0);
        }
        if (i == cmdNumbers - 1) {
            setOutDirect(&command[i], true, STDOUT_FILENO, 0);
        } else {
            setOutDirect(&command[i], true, fdw, 0);
        }
        if (isInputRedirect(&command[i])) {
            int fd = getInputRedirect(&command[i]);
            setInDirect(&command[i], true, fd, 0);
            ridInputRedirect(&command[i]);
        }
        if (isOutputRedirect(&command[i])) {
            int fd = getOutputRedirect(&command[i]);
            setOutDirect(&command[i], true, fd, 0);
            ridOutputRedirect(&command[i]);
        }
        int ret = execCommand(&command[i]);
        setExitState(ret);
        close(fdw);
        close(fdr);
    }

    for (size_t i = 0; i < cmdNumbers; i++) {
        freeCommand(&command[i]);
    }


}


bool isEmpty(char *str, size_t strLength) {
    for (size_t i = 0; i < strLength; i++) {
        if (str[i] > ' ')
            return false;
    }
    return true;
}

ssize_t readCommand(char *cmd) {
    if (fgets(cmd, MAX_LINE, stdin) == NULL) {
        return -1;
    }
    return strlen(cmd);
}

void printInfo() {
    printf("\033[32;1mmyshell\033[37m:\033[34m%s> \033[0m", getPath());
    fflush(stdout);
}