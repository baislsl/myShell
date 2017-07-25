#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>
#include "internal.h"
#include "parser.h"


ssize_t readCommand(char *cmd) {
    if (fgets(cmd, MAXLINE, stdin) == NULL) {
        return -1;
    } else {
        return strlen(cmd);
    }
}

void runCommand(char *cmd) {
    if(isEmpty(cmd, strlen(cmd)))
        return;
    char spiltCmd[MAX_PIPE + 1][MAXLENGTH];
    char *tmpSplitCmd[MAX_PIPE + 1];
    for (size_t i = 0; i < MAX_PIPE + 1; i++) {
        tmpSplitCmd[i] = spiltCmd[i];
    }
    ssize_t cmdNumbers = pipeSplit(cmd, strlen(cmd), tmpSplitCmd, MAX_PIPE + 1);
    int pipeFd[cmdNumbers - 1][2];
    for (size_t i = 0; i < cmdNumbers - 1; i++) {
        pipe(pipeFd[i]);
    }

    Command command[cmdNumbers];
    int fdr, fdw;
    char tmpFile1[MAXLINE], tmpFile2[MAXLINE];
    tmpnam(tmpFile1); tmpnam(tmpFile2);
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
            err_sys("build fail", STDOUT_FILENO);
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

void printInfo() {
    printf("myshell:%s> ", getPath());
    fflush(stdout);
}

void init() {
    setpath("/bin:/usr/bin");
    addPath(getPath());
}

int run(bool info) {
    char cmd[MAXLINE];
    int shouldRun = 1; /* flag to determine when to exit program */
    init();
    while (shouldRun) {
        if (info)
            printInfo();
        if (readCommand(cmd) == -1){
            shouldRun = 0;
            continue;
        }
        runCommand(cmd);
        fflush(stdout);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        int oldStdin = dup(STDIN_FILENO);
        for (int i = 1; i < argc; i++) {
            int fd = open(argv[i], O_RDONLY);
            if (fd == -1) {
                fprintf(stderr, "Fail to open file : %s\n", argv[i]);
                continue;
            }
            dup2(fd, STDIN_FILENO);
            run(false);
            close(fd);
        }
        dup2(oldStdin, STDIN_FILENO);
    } else {
        run(true);
    }
}