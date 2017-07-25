#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <fcntl.h>
#include "internal.h"
#include "command.h"
#include "parser.h"

int pipeFd[MAX_PIPE][2];

ssize_t readCommand(char *cmd) {
    ssize_t n = read(STDIN_FILENO, cmd, MAXLINE);
    if (n == -1)
        return -1;
    cmd[n - 1] = 0;
    return n;
}

void init() {
    printf("myshell> ");
    fflush(stdout);
    setpath("/bin:/usr/bin");
    addPath(getPath());
    for (size_t i = 0; i < MAX_PIPE; i++) {
        pipe(pipeFd[i]);
    }
}

void runCommand(char *cmd) {
    int pipeIndex[MAXLENGTH];
    char spiltCmd[MAX_PIPE + 1][MAXLENGTH];
    char *tmpSplitCmd[MAX_PIPE + 1];
    for (size_t i = 0; i < MAX_PIPE + 1; i++) {
        tmpSplitCmd[i] = spiltCmd[i];
    }
    ssize_t cmdNumbers = pipeSplit(cmd, strlen(cmd), tmpSplitCmd, MAX_PIPE + 1);
    // ssize_t cmdNumbers = getAllPipeIndex(cmd, strlen(cmd), pipeIndex, MAXLENGTH) + 1;
    int pipeFd[cmdNumbers - 1][2];
    for (size_t i = 0; i < cmdNumbers - 1; i++) {
        pipe(pipeFd[i]);
    }

    Command command[cmdNumbers];
    int fdr, fdw;
    for (size_t i = 0; i < cmdNumbers; i++) {
        if (i % 2 == 0) {
            fdw = open("pipe_w", O_WRONLY | O_TRUNC );
            fdr = open("pipe_r", O_RDONLY );
        } else {
            fdw = open("pipe_r", O_WRONLY | O_TRUNC);
            fdr = open("pipe_w", O_RDONLY );
        }
        int n = buildCmd(&command[i], spiltCmd[i], strlen(spiltCmd[i]));
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
        if(isInputRedirect(&command[i])){
            int fd = getInputRedirect(&command[i]);
            setInDirect(&command[i], true, fd, 0);
            ridInputRedirect(&command[i]);
        }
        if(isOutputRedirect(&command[i])){
            int fd = getOutputRedirect(&command[i]);
            setOutDirect(&command[i], true, fd, 0);
            ridOutputRedirect(&command[i]);
        }

        if (n < 0) {
            err_sys("build fail", STDOUT_FILENO);
        }
        execCommand(&command[i]);
        close(fdw);
        close(fdr);
    }


    for (size_t i = 0; i < cmdNumbers; i++) {
        freeCommand(&command[i]);
    }


}

int main(void) {
    char cmd[MAXLINE];
    int shouldRun = 1; /* flag to determine when to exit program */
    init();
    while (shouldRun) {
        printf("myshell:%s> ", getPath());
        fflush(stdout);
        if(readCommand(cmd) == -1)
            err_sys("error", STDOUT_FILENO);
        runCommand(cmd);
        fflush(stdout);
    }
    return 0;
}