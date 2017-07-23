//
// Created by baislsl on 17-7-23.
//

#include <memory.h>
#include "parser.h"


int findCharacter(char *str, size_t strLength, size_t index[], size_t indexSize, Func *func) {
    bool singleQuote = false, doubleQuote = false;
    size_t k = 0; // the index of index[]
    size_t i = 0;
    while (i < strLength) {
        if (func(str[i]) && !singleQuote && !doubleQuote) {
            if (k == indexSize)
                return -1;  // too much such character
            else {
                index[k++] = i++;
            }
        } else if (str[i] == '\'') {
            singleQuote = !singleQuote;
            ++i;
        } else if (str[i] == '\"') {
            doubleQuote = !doubleQuote;
            ++i;
        } else {
            ++i;
        }
    }
    return (int)k;
}

bool isSpace(char cc) {
    return cc <= ' ';
}

ssize_t ridFind(char *str, size_t strLength, char *store[], size_t storeSize, Func *func) {
    size_t index[storeSize];
    int size;
    if ((size = findCharacter(str, strLength, index, storeSize, func)) < 0) {
        return -1;
    }
    for (size_t i = 0; i <= size; i++) {
        size_t from = i == 0 ? 0 : index[i - 1] + 1;
        size_t to = (i == size) ? strLength : index[i];
        memmove(store[i], str + from, to - from);
    }
    return size + 1;
}


ssize_t spaceSplit(char *str, size_t strLength, char *store[], size_t storeSize) {
    return ridFind(str, strLength, store, storeSize, isSpace);
}


size_t getFirstArg(char *str, size_t strLength, char *dest) {
    size_t begin = 0, end;
    while (str[begin] <= ' ') ++begin;
    end = begin;
    while (str[end] > ' ') ++end;
    strncpy(dest, str + begin, end - begin);
    dest[end - begin] = 0;
    return end - begin;
}