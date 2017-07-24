#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include "internal.h"
#include "command.h"

int pipeFd[MAX_PIPE][2];

int setpath(char *newPath) {
    setenv("PATH", newPath, 1);
    // puts(getenv("PATH"));
}

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
    for (size_t i = 0; i < MAX_PIPE; i++) {
        pipe(pipeFd[i]);
    }
}

void runCommand(char *cmd) {
    int pipeIndex[MAXLENGTH];
    ssize_t cmdNumbers = getAllPipeIndex(cmd, strlen(cmd), pipeIndex, MAXLENGTH) + 1;
    int pipeFd[cmdNumbers - 1][2];
    for (size_t i = 0; i < cmdNumbers - 1; i++) {
        pipe(pipeFd[i]);
    }

    Command command[cmdNumbers];
    for (size_t i = 0; i < cmdNumbers; i++) {
        int from = i == 0 ? 0 : pipeIndex[i - 1] + 1;
        int to = (i == cmdNumbers - 1) ? (int) strlen(cmd) : pipeIndex[i];
        int n = buildCmd(command + i, cmd + from, to - from);
        if(i == 0){
            setInDirect(command + i, true, STDIN_FILENO, 0);
        }else{
            setInDirect(command + i, false, pipeFd[i - 1][0], pipeFd[i - 1][1]);
        }
        if(i == cmdNumbers - 1){
            setOutDirect(command + i, true, STDOUT_FILENO, 0);
        }else{
            setOutDirect(command + i, false, pipeFd[i][1], pipeFd[i][0]);
        }
        if(n < 0){
            err_sys("build fail", STDOUT_FILENO);
        }
    }

    for (size_t i = 0; i < cmdNumbers; i++) {
        execCommand(&command[i]);
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
        readCommand(cmd);
        runCommand(cmd);
        fflush(stdout);
    }
    return 0;
}