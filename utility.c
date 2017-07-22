//
// Created by baislsl on 17-7-21.
//

#include <glob.h>
#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>
#include "myshell.h"

void err_sys(char *msg){
    write(STDERR_FILENO, msg, strlen(msg));
}

int findCharacter(char *str, size_t strLength, size_t from[], size_t to[], size_t indexSize, Func *func) {
    bool singleQuote = false, doubleQuote = false;
    size_t index = 0; // the index of index[]
    from[0] = 0;
    size_t i = 0;
    while (i < strLength) {
        if (func(str[i]) && !singleQuote && !doubleQuote) {
            if (index == indexSize)
                return -1;  // too much such character
            else {
                to[index++] = i;
                if (i == 0) {
                    --index;
                }
                do{
                    ++i;
                }while(func(str[i]));
                if(index != indexSize)
                    from[index] = i;
            }
        } else if (str[i] == '\'') {
            singleQuote = !singleQuote;
            ++i;
        } else if (str[i] == '\"') {
            doubleQuote = !doubleQuote;
            ++i;
        }else{
            ++i;
        }
    }
    to[index++] = i;
    return (int) index;
}


bool isSpace(char cc) {
    return cc <= ' ';
}


ssize_t ridFind(char *str, size_t strLength, char *store[], size_t storeSize, Func *func) {
    size_t from[storeSize], to[storeSize];
    int size;
    if ((size = findCharacter(str, strLength, from, to, storeSize, func)) < 0) {
        return -1;
    }
    for (size_t i = 0; i < size; i++) {
        memmove(store[i], str + from[i], to[i] - from[i]);
    }
    return size;
}

ssize_t spaceSplit(char *str, size_t strLength, char *store[], size_t storeSize) {
    return ridFind(str, strLength, store, storeSize, isSpace);
}


void utilTest() {
    char *a = "while return     gg, \" sh\" ,kk";
    char *store[10];
    for (int i = 0; i < 10; i++) {
        store[i] = (char *) malloc(10);
    }
    ssize_t n = spaceSplit(a, strlen(a), store, 10);
    for (int i = 0; i < n; i++) {
        printf("%s--\n", store[i]);
    }
}
