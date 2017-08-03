//
// Created by baislsl on 17-7-22.
//

#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <fcntl.h>
#include "internal.h"
#include "command.h"
#include "parser.h"

#include <stdio.h>

extern char** environ;

int main(int argc, char* argv[])
{
    int i;

    for (i = 0; environ[i] != NULL; i++)
    {
        printf("%s\n", environ[i]);
    }

    return 0;
}

void func1() {
    int fd = open("pipe_r", O_RDONLY);
    dup2(fd, STDIN_FILENO);

    // fopen("pipe_r", "r", stdin);
    char line[1000];
    gets(line);
    puts(line);
    char *avg[] = {"cat"};
    execvp("cat", avg);
    close(fd);
    puts("gg");
}

//int runOuterCmd(CommandPtr cmd) {
//    int fd[2];
//    pipe(fd);
//    int pid = fork();
//    if (pid < 0) {
//        printf("Error when fork a child process");
//    } else if (pid > 0) { // parent
//        close(fd[1]);
//        char background[2];
//        read(fd[0], background, 1);
//        if (background[0] == 'y') {
//        } else {
//            waitpid(pid, NULL, 0);
//        }
//    } else {  // child
//        close(fd[0]);
//        if (isBackground(cmd)) {
//            write(fd[1], "y", 1);
//        } else {
//            write(fd[1], "n", 1);
//        }
//        int ok = normalCmd(cmd, cmdLength, inFd, outFd, pid);
//        if (!ok) {
//            // shouldRun = 0;
//        }
//        exit(0);
//    }
//}
//
//int normalCmd(char *cmd, size_t cmdLength, int inFd, int outFd, int fork) {
//    dup2(inFd, STDIN_FILENO);
//    dup2(outFd, STDOUT_FILENO);
//
//    char args_data[MAXLINE / 2 + 1][MAXLENGTH];
//    char *args[MAXLINE / 2 + 1];
//    for (int i = 0; i < MAXLINE / 2 + 1; i++) {
//        args[i] = args_data[i];
//    }
//    ssize_t n = spaceSplit(cmd, cmdLength, args, MAXLINE / 2 + 1);
//
//    if (n < 0) {
//        err_sys("to many arguments", outFd);
//    } else {
//        for (size_t i = n; i < MAXLINE / 2 + 1; i++) {
//            args[i] = NULL;
//        }
//        execvp(args[0], args);
//    }
//
//    close(inFd);
//    close(outFd);
//}


/*int runOuterCmd2(CommandPtr cmd) {
    int status;
    pid_t pid;
    int gpid = tcgetpgrp(STDIN_FILENO), gpid2 = tcgetpgrp(STDOUT_FILENO);
    bool background = isBackground(cmd);
    if (background)
        ridBackgroundChar(cmd);
    if ((pid = fork()) < 0) {
        err_sys("Fork error", STDERR_FILENO);
    } else if (pid == 0) { // child
        if (!background) {
            setpgid(0, getpid());
        }
        execvp(cmd->argv[0], cmd->argv);
        tcsetpgrp(STDIN_FILENO, getppid()); // 设置当前 group 为 active
    } else {  // parent
        if (background) {
            addPid(pid, cmd);
            if (waitpid(pid, &status, WNOHANG) != pid) {
                err_sys("wait error\n", STDERR_FILENO);
                return -1;
            }
        } else {
            // tcsetpgrp(STDOUT_FILENO, pid); // 设置当前 group 为 active
            tcsetpgrp(STDIN_FILENO, pid); // 设置当前 group 为 active
            if (waitpid(pid, &status, 0) != pid) {
                err_sys("wait error\n", STDERR_FILENO);
                return -1;
            }

            if (WIFEXITED(status)) {

            } else if (WIFSTOPPED(status)) {

            }

        }
    }
}

int runOuterCmd3(CommandPtr cmd) {
    int status;
    bool background = isBackground(cmd);
    if (background) {
        ridBackgroundChar(cmd);
    }
    int pid = fork();
    if (pid < 0) {
        err_sys("fork error", STDOUT_FILENO);
    } else if (pid == 0) {

        // processGroups.add(mypid);
        // exec(cmd); // 运行命令

        if (background) {
            handleBackExit2(getpid());
        }
        execvp(cmd->argv[0], cmd->argv);
        // tcsetpgrp(STDOUT_FILENO, getppid()); // 设置当前 group 为 active
        tcsetpgrp(STDIN_FILENO, getppid()); // 设置当前 group 为 active

    } else {
        if (background) {
            addPid(pid, cmd);
            if (waitpid(pid, &status, WNOHANG) != pid) {
                err_sys("wait error for background process\n", STDERR_FILENO);
            }
        } else {
            // tcsetpgrp(STDOUT_FILENO, getpgrp()); // 拿回 active process group
            tcsetpgrp(STDIN_FILENO, getpgrp()); // 拿回 active process group
            if (waitpid(pid, &status, 0) != pid) {
                err_sys("wait error\n", STDERR_FILENO);
            }
            //  tcsetpgrp(STDOUT_FILENO, getpgrp()); // 拿回 active process group
        }


        return status;
    }
}

int runOuterCmd(CommandPtr cmd) {
    testAll();
    int status;
    bool background = isBackground(cmd);
    if (background) {
        ridBackgroundChar(cmd);
    }
    int pid = fork();
    if (pid < 0) {
        err_sys("fork error", STDOUT_FILENO);
    } else if (pid == 0) {
        if (background) {

        }
        execvp(cmd->argv[0], cmd->argv);
    } else {
        if (background) {
            addPid(pid, cmd);
            if (waitpid(pid, &status, WNOHANG) != pid) {
            }
        } else {
            forePid = pid;
            fprintf(stdout, "set forepid to %d\n ", pid);
            if (waitpid(pid, &status, WUNTRACED) != pid) {
            }
            fprintf(stdout, "reset forepid to -1\n ");
            forePid = -1;
        }
        //resetDirect(cmd);
        return 0;
    }
}*/
