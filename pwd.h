//
// Created by baislsl on 17-7-22.
//

#ifndef MYSHELL_PWD_H
#define MYSHELL_PWD_H

#include "internal.h"
#define MAXPATH 4096

int pwd(const char *args[], size_t n, int outDf) {
    if (n != 1) {
        err_sys("gg pwd\n", outDf);
    } else {
        char path[MAXPATH];
        if(getcwd(path, MAXPATH) == NULL){
            err_sys("get path fail", outDf);
        }
        write(outDf, path, strlen(path));
        write(outDf, "\n", 1);
    }
}

#endif //MYSHELL_PWD_H
