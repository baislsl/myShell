//
// Created by baislsl on 17-8-3.
//

#include <malloc.h>
#include <string.h>
#include "param.h"

// lastExitState : the exit state of the last command
int lastExitState = 0;

int argc;       // argument number
char **argv;    // arguments

// get the argument number
int getArgc() {
    return argc;
}

// initialize the argc and argv
void paramInit(int _argc, char *_argv[]) {
    // argv exit all along the life of the whole program
    // use function malloc to allocate space fo argv
    // and do not free until shift
    argc = _argc;
    argv = (char **) malloc(argc * sizeof(char *));
    for (int i = 0; i < argc; i++) {
        argv[i] = (char *) malloc(sizeof(_argv[i]) + 1);
        strcpy(argv[i], _argv[i]);
    }
}

// set the exit state of last command
void setExitState(int state) {
    lastExitState = state;
}

// get the exit state of last command
int getExitState() {
    return lastExitState;
}

/**
 * get the argument of index,
 * return NULL if index > argc
 * */
const char *param(size_t index) {
    if (index > argc)
        return NULL;
    return argv[index];
}

/**
 * shift the argument
 *
 * @return 1 if i > argc, else return 0
 * */
int shift(size_t i) {
    int ret = i > argc ? 1 : 0;
    while (i-- && argc--) {
        free(argv[i]);
        ++argv;
    }
    return ret;
}

// print all the argument at address dest
void printAllParam(char *dest) {
    dest[0] = 0;    // set dest to an empty string
    for (size_t i = 0; i < argc; i++) {
        strcat(dest, argv[i]);
        if (i != argc - 1)
            strcat(dest, " ");
    }
}
