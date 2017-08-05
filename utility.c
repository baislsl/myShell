//
// Created by baislsl on 17-7-21.
//

#include <fcntl.h>
#include <sys/stat.h>
#include "utility.h"
#include "command.h"
#include "param.h"
#include "parser.h"

// get current path
char *getPath() {
    static char path[PATH_MAX];
    getcwd(path, PATH_MAX);
    return path;
}

// set environment variable $PATH
// this function will overwrite the previous $PATH
int setpath(char *newPath) {
    setenv("PATH", newPath, 1);
}

// puts msg to stderr in red color
void err_sys(char *msg) {
    fprintf(stderr, "\033[31;1m%s\033[0m", msg);
}

/**
 * run a command given as string
 * handle '|', '&'
 *
 * For pipe, use temp file as virtual pipe
 * before set redirect for '&', the command is as the form :
 *      cmd1 | cmd2 | cmd3 |...| cmdn
 * and the input output direct is
 *      stdin -> cmd1 -> cmd2 -> cmd3 -> ... -> cmdn -> stdout
 * use temp file to act the bridge between command.
 *
 * */
void runCommand(char *cmd) {
    if (isEmpty(cmd, strlen(cmd)))
        return;
    char spiltCmd[MAX_PID + 1][MAX_LENGTH];
    char *tmpSplitCmd[MAX_PID + 1];
    for (size_t i = 0; i < MAX_PID + 1; i++) {
        tmpSplitCmd[i] = spiltCmd[i];
    }

    // split the command according to '|'
    // cmdNumbers is the number of command after spilt by '|'
    ssize_t cmdNumbers = pipeSplit(cmd, strlen(cmd), tmpSplitCmd, MAX_PID);

    Command command[cmdNumbers];
    int fdr, fdw;
    char tmpFile1[MAX_LINE], tmpFile2[MAX_LINE];

    // get two temp file name
    tmpnam(tmpFile1);
    tmpnam(tmpFile2);
    for (size_t i = 0; i < cmdNumbers; i++) {
        // the output temp file will turn to input temp file for the next command
        // the input temp file will be clear and turn to output temp file for the next command
        // Thus, we use 2 as a cycle to swap two temp file.
        // In function open, parameter O_TRUNC is used because before each write,
        // we need to clear up the contain of the file
        if (i % 2 == 0) {
            fdw = open(tmpFile1, O_CREAT | O_WRONLY | O_TRUNC);
            fdr = open(tmpFile2, O_CREAT | O_RDONLY);
        } else {
            fdw = open(tmpFile2, O_CREAT | O_WRONLY | O_TRUNC);
            fdr = open(tmpFile1, O_CREAT | O_RDONLY);
        }

        int n = buildCmd(&command[i], spiltCmd[i], strlen(spiltCmd[i]));
        if (n < 0) {
            err_sys("Error pipe form");
        }

        // set the direct according to pipe
        if (i == 0) {
            setInDirect(&command[i], STDIN_FILENO);
        } else {
            setInDirect(&command[i], fdr);
        }
        if (i == cmdNumbers - 1) {
            setOutDirect(&command[i], STDOUT_FILENO);
        } else {
            setOutDirect(&command[i], fdw);
        }

        // after setting the pipe direct, set the redirect declared in each command if exist
        if (isInputRedirect(&command[i])) {
            int fd = getInputRedirect(&command[i]);
            if(fd != 0){
                setInDirect(&command[i], fd);
                ridInputRedirect(&command[i]);
            }
        }
        if (isOutputRedirect(&command[i])) {
            int fd = getOutputRedirect(&command[i]);
            setOutDirect(&command[i], fd);
            ridOutputRedirect(&command[i]);
        }

        execCommand(&command[i]);
        close(fdw);
        close(fdr);
    }

    // free all the command malloc in buildCmd
    for (size_t i = 0; i < cmdNumbers; i++) {
        freeCommand(&command[i]);
    }
}

// judge whether a given string is empty
bool isEmpty(const char *str, size_t strLength) {
    for (size_t i = 0; i < strLength; i++) {
        if (!isSpace(str[i]))
            return false;
    }
    return true;
}

// read line from sdtin and return the size of the line
ssize_t readCommand(char *cmd) {
    if (fgets(cmd, MAX_LINE, stdin) == NULL) {
        return -1;
    }
    return strlen(cmd);
}

// print prompting message of the shell
void printInfo() {
    printf("\033[32;1mmyshell\033[37m:\033[34m%s> \033[0m", getPath());
    fflush(stdout);
}

// return the umask
mode_t getUmask(){
    mode_t mode = umask(0);
    umask(mode);
    return mode;
}
