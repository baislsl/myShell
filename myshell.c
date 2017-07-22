#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include <wait.h>
#include "myshell.h"
#include "cd.h"
#include "pwd.h"

#define MAXLINE 80 /* The maximum length command */
#define MAXPATH 256
#define MAXLENGTH 4096

char *path[MAXPATH];
size_t pathIndex;

bool isPathDiv(char cc) {
    return cc == ':';
}

int setpath2(char *newPath) {
    setenv("PATH", newPath, 1);
    // puts(getenv("PATH"));
}

int setpath(char *newPath) {
    char *add[MAXPATH - pathIndex];
    for (int i = 0; i < MAXPATH - pathIndex; i++) {
        add[i] = (char *) malloc(MAXLINE);
    }

    ssize_t n = ridFind(newPath, strlen(newPath), add, MAXPATH - pathIndex, isPathDiv);
    if (n < 0) {
        for (int i = 0; i < MAXPATH - pathIndex; i++)
            free(add[i]);
        return -1;
    }
    for (ssize_t i = 0; i < n; i++) {
        path[pathIndex + i] = add[i];
    }
    for (ssize_t i = n; i < MAXPATH - pathIndex; i++) {
        free(add[i]);
    }
    return n;
}

ssize_t readCommand(char *cmd) {
    ssize_t n = read(STDIN_FILENO, cmd, MAXLINE);
    if (n == -1)
        return -1;
    cmd[n - 1] = 0;
    return n;
}

bool isBackground(char *cmd) {

}

int isInternalCmd(char *cmd, size_t cmdLength) {
    return false;
}

int isPipe(char *cmd, size_t cmdLength) {
    return false;
}


int isIoRedirect(char *cmd, size_t cmdLength) {
    return false;
}

void init() {
    printf("myshell> ");
    fflush(stdout);
    setpath2("/bin:/usr/bin");
}


int normalCmd(char *cmd, size_t cmdLength, int inFd, int outFd, int fork) {
    dup2(inFd, STDIN_FILENO);
    dup2(outFd, STDOUT_FILENO);


    char args_data[MAXLINE / 2 + 1][MAXLENGTH];
    char *args[MAXLINE / 2 + 1];
    for (int i = 0; i < MAXLINE / 2 + 1; i++) {
        args[i] = args_data[i];
    }

    ssize_t n = spaceSplit(cmd, cmdLength, args, MAXLINE / 2 + 1);

    if (n < 0) {
        err_sys("to many arguments", outFd);
    } else {
        for (size_t i = n; i < MAXLINE / 2 + 1; i++) {
            args[i] = NULL;
        }
        execvp(args[0], args);
    }

    close(inFd);
    close(outFd);
}

int runInternalCmd(char *cmd, size_t cmdLength, int inFd, int outFd) {
    char *args[MAXLINE / 2 + 1]; /* command line arguments */
    for (int i = 0; i < MAXLINE / 2 + 1; i++) {
        args[i] = (char *) malloc(MAXLINE);
    }
    const ssize_t n = spaceSplit(cmd, strlen(cmd), args, MAXLINE / 2 + 1);
    if (isInternalCmd(cmd, strlen(cmd))) {
        if (strcmp(args[0], "cd") == 0) {
            cd(args, n, outFd);
        } else if (strcmp(args[0], "pwd") == 0) {
            pwd(args, n, outFd);
        }
    }
    return 0;
}

void runOuterCmd(char *cmd, size_t cmdLength, int inFd, int outFd) {
    int fd[2];
    pipe(fd);
    int pid = fork();
    if (pid < 0) {
        printf("Error when fork a child process");
    } else if (pid > 0) { // parent
        close(fd[1]);
        char background[2];
        read(fd[0], background, 1);
        if (background[0] == 'y') {
        } else {
            waitpid(pid, NULL, 0);
        }
    } else {  // child
        close(fd[0]);
        if (isBackground(cmd)) {
            write(fd[1], "y", 1);
        } else {
            write(fd[1], "n", 1);
        }
        int ok = normalCmd(cmd, cmdLength, inFd, outFd, pid);
        if (!ok) {
            // shouldRun = 0;
        }
        exit(0);
    }
}

void runCommand(char *cmd) {
    int outFd = STDOUT_FILENO, inFd = STDIN_FILENO;

    if (isPipe(cmd, strlen(cmd))) {
        return;
    }
    if (isInternalCmd(cmd, strlen(cmd))) {
        runInternalCmd(cmd, strlen(cmd), inFd, outFd);
    } else {
        runOuterCmd(cmd, strlen(cmd), inFd, outFd);
    }


}

int main(void) {
    char *args[MAXLINE / 2 + 1]; /* command line arguments */
    char cmd[MAXLINE];
    int shouldRun = 1; /* flag to determine when to exit program */
    init();
    while (shouldRun) {
        readCommand(cmd);
        runCommand(cmd);
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
    }
    return 0;
}