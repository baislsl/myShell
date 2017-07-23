//
// Created by baislsl on 17-7-22.
//

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include <wait.h>

int main(){
    char *avg[] = {"ls", "-al"};
    execvp("ls", avg);
    puts("gg");
}

int runOuterCmd(CommandPtr cmd) {
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
