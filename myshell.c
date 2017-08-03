#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include "internal.h"
#include "parser.h"
#include "forePid.h"
#include "myshell.h"
#include "utility.h"

void ExitSignal(int signal) {
    int forePid = getForePid();
    fprintf(stdout, "fore pid : %d\n", forePid);
    if (forePid != -1) {
        kill(forePid, signal);
    }
}

void sig_tstp(int signal){
    fprintf(stdout, "call sig tstp\n");
    ExitSignal(SIGSTOP);
}

void sig_quit(int signal) {
    fprintf(stdout, "call quit\n");
    exit(0);
}

void sig_int(int signal) {
    int forePid = getForePid();
    puts("call int");
    fprintf(stdout, "fore pid : %d\n", forePid);
    ExitSignal(signal);
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

void init() {
    signal(SIGQUIT, sig_quit);
    signal(SIGTSTP, sig_tstp);
    signal(SIGINT, sig_int);
    setpath("/bin:/usr/bin");
    addPath(getPath());
}

int run(bool info) {
    char cmd[MAX_LINE];
    int shouldRun = 1; /* flag to determine when to exit program */
    init();
    while (shouldRun) {
        if (info)
            printInfo();
        if (readCommand(cmd) == -1) {
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
                char msg[MAX_LENGTH];
                perror("Error:");
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