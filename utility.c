//
// Created by baislsl on 17-7-21.
//

#include <fcntl.h>
#include "utility.h"
#include "parser.h"

int setpath(char *newPath) {
    setenv("PATH", newPath, 1);
    // puts(getenv("PATH"));
}

int addPath(char *newPath) {
    char *env = getenv("PATH");
    char path[MAXLENGTH];
    strcpy(path, env);
    size_t len = strlen(path);
    path[len] = ':';
    path[len + 1] = 0;
    strcat(path, newPath);
    setenv("PATH", path, 1);
    return 0;
}

void err_sys(char *msg, int outFd) {
    write(outFd, msg, strlen(msg));
}

bool isEmpty(char *str, size_t strLength) {
    for (size_t i = 0; i < strLength; i++) {
        if (str[i] > ' ')
            return false;
    }
    return true;
}

// still a bug version
ssize_t isPipe(char *str, size_t strLength) {
    if (isEmpty(str, strLength)) return -1;
    for (ssize_t i = 0; i < strLength; i++) {
        if (str[i] == '|')
            return i;
    }
    return strLength;
}


int isIoRedirect(char *str, size_t strLength) {
    for (ssize_t i = 0; i < strLength; i++) {
        if (str[i] == '<' || str[i] == '>')
            return i;
    }
    return -1;
}


void utilTest() {
    // char *a = "while return     gg, \" sh\" ,kk";
    char *a = "pwd";
    char *store[10];
    for (int i = 0; i < 10; i++) {
        store[i] = (char *) malloc(10);
    }
    ssize_t n = spaceSplit(a, strlen(a), store, 10);
    for (int i = 0; i < n; i++) {
        printf("%s--\n", store[i]);
    }
}