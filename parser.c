//
// Created by baislsl on 17-7-23.
//

#include <memory.h>
#include "parser.h"


/**
 * split the str by character that satisfied func
 * and store the splitted strings in store
 * This function can escape character in "" and ''
 * and must not use this to split character ' and "
 *
 * @return return the number of splitted string,
 * return -1 if the string contain open quote
 * return -2 if the size if bigger than storeSize
 * */
ssize_t split(char *str, size_t strLength, char *store[], size_t storeSize, Func *func) {
    size_t i = 0;
    size_t begin, end;
    size_t storeIndex = 0;
    while (i < strLength) {
        // rid all the continuous character satisfied func
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
        // find all the continuous character don't satisfied func
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
        // store one splitted string to store[]
        if (storeIndex == storeSize) {
            return -2;              // too big splitted number
        }
        if (end != begin) {
            strncpy(store[storeIndex], str + begin, end - begin);
            store[storeIndex++][end - begin] = 0;
        }
    }
    return storeIndex;
}

/**
 * find all the character in str that satisfied func
 * and store their index in array index[]
 * This function can escape character in "" and ''
 * and must not use this to find character ' and "
 * @return return the number of satisfied character
 * */
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


// judge where cc is space, tab ...
bool isSpace(char cc) {
    return cc <= ' ';
}

bool isPipeCharacter(char cc) {
    return cc == '|';
}

/**
 * split the given str by '|'
 * @return the number of command after divide by '|'
 * */
ssize_t pipeSplit(char *str, size_t strLength, char *store[], size_t storeSize) {
    return split(str, strLength, store, storeSize, isPipeCharacter);
}

/**
 * split the given str by space, tab...
 * @return the number of command after divide by space, tab...
 * */
ssize_t spaceSplit(char *str, size_t strLength, char *store[], size_t storeSize) {
    return split(str, strLength, store, storeSize, isSpace);
}

