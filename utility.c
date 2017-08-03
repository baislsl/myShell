//
// Created by baislsl on 17-7-21.
//

#include <fcntl.h>
#include "utility.h"
#include "parser.h"
#include "command.h"


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
            //setInDirect(command + i, false, pipeFd[i - 1][0], pipeFd[i - 1][1]);
            setInDirect(&command[i], true, fdr, 0);
        }
        if (i == cmdNumbers - 1) {
            setOutDirect(&command[i], true, STDOUT_FILENO, 0);
        } else {
            // setOutDirect(command + i, false, pipeFd[i][1], pipeFd[i][0]);
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
        execCommand(&command[i]);
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

// still a bug version
ssize_t isPipe(char *str, size_t strLength) {
    if (isEmpty(str, strLength)) return -1;
    for (ssize_t i = 0; i < strLength; i++) {
        if (str[i] == '|')
            return i;
    }
    return strLength;
}


int isIoRedirect(char *str, size_t strLength) {
    for (ssize_t i = 0; i < strLength; i++) {
        if (str[i] == '<' || str[i] == '>')
            return i;
    }
    return -1;
}


void utilTest() {
    // char *a = "while return     gg, \" sh\" ,kk";
    char *a = "pwd";
    char *store[10];
    for (int i = 0; i < 10; i++) {
        store[i] = (char *) malloc(10);
    }
    ssize_t n = spaceSplit(a, strlen(a), store, 10);
    for (int i = 0; i < n; i++) {
        printf("%s--\n", store[i]);
    }
}