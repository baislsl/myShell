//
// Created by baislsl on 17-7-22.
//

#include "internal.h"
#include "pwd.h"

int cd(const char *args[], const size_t n, int outFd) {
    if (n != 2) {
        err_sys("argument for cd must be 2\n", outFd);
        return -1;
    } else {
        int ret = chdir(args[1]);
        if (ret < 0) {
            err_sys("No such path\n", outFd);
            pwd(NULL, 1, outFd);
        }
        return ret;
    }
}