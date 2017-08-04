#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "forePid.h"
#include "myshell.h"
#include "utility.h"
#include "param.h"

// initialization of the program
void init(int argc, char *argv[]) {
    // catch SIGQUIT, SIGTSTP, SIGINT
    signal(SIGQUIT, sig_quit);
    signal(SIGTSTP, sig_tstp);
    signal(SIGINT, sig_int);

    // set $PATH
    setpath("/bin:/usr/bin");

    // add environment:
    //    shell=<pathname>/myshell
    //    parent=<pathname>/myshell
    char *path = getPath();
    char *shell = (char*)malloc(MAX_LENGTH);
    sprintf(shell, "shell=%s", path);
    putenv(shell);
    shell = (char*)malloc(MAX_LENGTH);
    sprintf(shell, "parent=%s", path);
    putenv(shell);

    // initialize parameter
    paramInit(argc, argv);
}

/**
 * read and execute command
 *
 * @param info print the shell prompt if info is true
 *      when execute command from file, info is false
 *      when execute command from stdin, info is true
 * */
int run(bool info) {
    char cmd[MAX_LINE];

    // two exit for while
    //   1. readCommand read EOF from input
    //   2. quit/exit command executed
    while (1) {
        if (info) {
            printInfo();
        }
        if (readCommand(cmd) == -1) {
            break;
        }
        runCommand(cmd);
        fflush(stdout);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    init(argc, argv);
    if (argc >= 2) {        // read command from file
        int oldInFd = dup(STDIN_FILENO);
        for (int i = 1; i < argc; i++) {
            int fd = open(argv[i], O_RDONLY);
            if (fd == -1) {
                perror("Error:");
                continue;
            }
            dup2(fd, STDIN_FILENO);
            run(false);
            close(fd);
        }
        dup2(oldInFd, STDIN_FILENO);
    } else {
        run(true);
    }
}
