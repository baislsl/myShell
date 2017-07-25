//
// Created by baislsl on 17-7-23.
//

#include <memory.h>
#include "parser.h"


// not finished
int escapeQuote(const char *str, size_t *i, size_t maxLength) {
    if (str[*i] == '\'' || str[*i] == '\"') {
        char cc = str[*i];
        ++i;
        while (*i < maxLength && str[*i] != cc) {
            *i = *i + 1;
        }
        if (*i == maxLength)  // error for not enclose quote
            return -1;
    } else {
        *i = *i + 1;
    }
}

ssize_t split(char *str, size_t strLength, char *store[], size_t storeSize, Func *func) {
    size_t i = 0;
    size_t begin, end;
    size_t storeIndex = 0;
    while (i < strLength) {
        while (i < strLength && func(str[i])) {
            if (str[i] == '\'' || str[i] == '\"') {
                char cc = str[i];
                ++i;
                while (i < strLength && str[i++] != cc);
                if (i == strLength)  // error for not enclose quote
                    return -1;
            } else {
                ++i;
            }
        }
        begin = i;
        while (i < strLength && !func(str[i])) {
            if (str[i] == '\'' || str[i] == '\"') {
                char cc = str[i];
                ++i;
                while (i < strLength && str[i++] != cc);
                if (i == strLength)  // error for not enclose quote
                    return -1;
            } else {
                ++i;
            }
        }
        end = i;
        if (storeIndex == storeSize) {
            return -2;
        } else if (begin != end) {
            strncpy(store[storeIndex], str + begin, end - begin);
            store[storeIndex++][end - begin] = 0;
        }
    }
    return storeIndex;
}

int findCharacter(char *str, size_t strLength, size_t index[], size_t indexSize, Func *func) {
    bool singleQuote = false, doubleQuote = false;
    size_t k = 0; // the index of index[]
    size_t i = 0;

    while (i < strLength && func(str[i])) ++i;   // trim the begin

    while (i < strLength) {
        if (func(str[i]) && !singleQuote && !doubleQuote) {
            if (k == indexSize)
                return -1;  // too much such character
            index[k++] = i;
            while (i < strLength && func(str[i])) ++i;

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
    return (int) k;
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
        memcpy(store[i], str + from, to - from);
        store[i][to - from] = 0;
    }
    return size + 1;
}

bool isPipeCharacter(char cc){
    return cc == '|';
}

ssize_t pipeSplit(char *str, size_t strLength, char *store[], size_t storeSize){
    return split(str, strLength, store, storeSize, isPipeCharacter);
}

ssize_t getAllPipeIndex(char *cmd, size_t cmdLength, int index[], size_t maxIndex){
    return findCharacter(cmd, cmdLength, (size_t*)index, maxIndex, isPipeCharacter);
}

ssize_t spaceSplit(char *str, size_t strLength, char *store[], size_t storeSize) {
    // return ridFind(str, strLength, store, storeSize, isSpace);
    return split(str, strLength, store, storeSize, isSpace);
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
