//
// Created by baislsl on 17-7-23.
//
#include "internal.h"

int pwd(const char *args[], size_t n) {
    if (n != 1) {
        err_sys("gg pwd\n", STDOUT_FILENO);
    } else {
        char path[MAXPATH];
        if(getcwd(path, MAXPATH) == NULL){
            err_sys("get path fail", STDOUT_FILENO);
        }
        write(STDOUT_FILENO, path, strlen(path));
        write(STDOUT_FILENO, "\n", 2);
    }
}


int cd(const char *args[], size_t n) {
    if(n == 1){
        return chdir(getenv("HOME"));
    }
    if (n != 2) {
        err_sys("argument for cd must be 2\n", STDOUT_FILENO);
        return -1;
    } else {
        int ret = chdir(args[1]);
        if (ret < 0) {
            err_sys("No such path\n", STDOUT_FILENO);
            pwd(NULL, 1);
        }
        return ret;
    }
}




