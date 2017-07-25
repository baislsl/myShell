//
// Created by baislsl on 17-7-23.
//
#include <dirent.h>
#include "internal.h"
#define MSG_LENGTH 1024

int pwd(const char *args[], size_t n) {
    if (n != 1) {
        err_sys("gg pwd\n", STDERR_FILENO);
        return -1;
    }
    char path[MAXPATH];
    if (getcwd(path, MAXPATH) == NULL) {
        err_sys("get path fail", STDERR_FILENO);
    }
    write(STDOUT_FILENO, path, strlen(path));
    write(STDOUT_FILENO, "\n", 2);
    return 0;
}

int cd(const char *args[], size_t n) {
    if (n == 1) {
        return chdir(getenv("HOME"));
    }
    if (n != 2) {
        err_sys("argument for cd must be 2\n", STDERR_FILENO);
        return -1;
    } else {
        if (chdir(args[1]) < 0) {
            err_sys("No such path\n", STDERR_FILENO);
            pwd(NULL, 1);
            return -1;
        }
        return 0;
    }
}


int time(const char *args[], size_t n) {

}

int clr(const char *args[], size_t n) {
    static char output[] = "\033[1A\033[2J\033[H";
    if (write(STDOUT_FILENO, output, strlen(output)) == -1)
        return -1;
    else
        return 0;
}

int dirPath(const char *path) {
    DIR *dir;
    if ((dir = opendir(path)) == NULL) {
        char msg[MSG_LENGTH];
        sprintf(msg, "Can not open directory : %s\n", path);
        err_sys(msg, STDERR_FILENO);
        return -1;
    }
    struct dirent *file;
    while ((file = readdir(dir)) != NULL) {
        write(STDOUT_FILENO, file->d_name, strlen(file->d_name));
        write(STDOUT_FILENO, " ", 1);
    }
    write(STDOUT_FILENO, "\n", 1);
}

/**
 * list arg path
 * if no, list current path
 * do not support "-x" yet
  * */
int dir(const char *args[], size_t n) {
    if (n == 1) { // list current path
        return dirPath(getPath());
    }
    char info[MSG_LENGTH];
    for (int i = 1; i < n; i++) {
        if(n != 2){
            sprintf(info, "%s:\n", args[i]);
            write(STDOUT_FILENO, info, strlen(info));
        }
        dirPath(args[i]);
    }
}

int environ(const char *args[], size_t n) {
    puts(getenv("PATH"));
}

int echo(const char *args[], size_t n) {

}

int help(const char *args[], size_t n) {

}

int quit(const char *args[], size_t n) {
    exit(0);
}

int umask(const char *args[], size_t n) {
}

struct tie {
    innerFunc *func;
    char *cmd;
};
typedef struct tie innerCmd;

int execInner(char *name, const char *args[], size_t argc){
    static innerCmd innerFuncList[] = {
            {cd,  "cd"},
            {pwd, "pwd"},
            {clr, "clr"},
            {dir, "dir"},
            {environ, "environ"},
            {echo, "echo"},
            {help, "help"},
            {quit, "quit"}
    };
    for (int i = 0;i<100;i++) {
        innerCmd *icmd = innerFuncList + i;
        if (strcmp(icmd->cmd, name) == 0) {
            return (icmd->func)(args, argc);
        }
    }
    return -1;
}

