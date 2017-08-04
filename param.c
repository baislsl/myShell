//
// Created by baislsl on 17-8-3.
//

#include <malloc.h>
#include <string.h>
#include "param.h"

int lastExitState = 0;
int argc;
char **argv;

int getArgc(){
    return argc;
}

void paramInit(int _argc, char *_argv[]) {
    argc = _argc;
    argv = (char **) malloc(argc * sizeof(char *));
    for (int i = 0; i < argc; i++) {
        argv[i] = (char *) malloc(sizeof(_argv[i]) + 1);
        strcpy(argv[i], _argv[i]);
    }
}

void setExitState(int state) {
    lastExitState = state;
}

int getExitState() {
    return lastExitState;
}

const char *param(size_t index) {
    if (index > argc)
        return NULL;
    return argv[index];
}


// return 1 if i > argc, else return 0
int shift(size_t i) {
    int ret = i > argc ? 1 : 0;
    while(i-- && argc--){
        free(argv[i]);
        ++argv;
    }
    return ret;


}
void printAllParam(char *dest){
    dest[0] = 0;
    for(size_t i =0;i<argc;i++){
        strcat(dest, argv[i]);
        if(i != argc - 1)
            strcat(dest, " ");
    }
}
