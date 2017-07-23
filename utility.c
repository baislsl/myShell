//
// Created by baislsl on 17-7-21.
//

#include "utility.h"
#include "parser.h"

void err_sys(char *msg, int outFd){
    write(outFd, msg, strlen(msg));
}

bool isEmpty(char *str, size_t strLength){
    for(size_t i =0;i<strLength;i++){
        if(str[i] > ' ')
            return false;
    }
    return true;
}

// still a bug version
ssize_t isPipe(char *str, size_t strLength) {
    if(isEmpty(str, strLength)) return -1;
    for(ssize_t i = 0;i<strLength; i++){
        if(str[i] == '|')
            return i;
    }
    return strLength;
}

int isInputRedirect(char *str, size_t strLength){
    for(ssize_t i = 0;i<strLength; i++){
        if(str[i] == '<')
            return i;
    }
    return -1;
}

int isOutputRedirect(char *str, size_t strLength){
    for(ssize_t i = 0;i<strLength; i++){
        if(str[i] == '>')
            return i;
    }
    return -1;
}

bool isBackground(char *cmd) {

}
//: bg、 cd 、 continue、 echo 、 exec 、
// exit 、fg 、jobs 、pwd 、set 、shift 、test 、time 、umask 、unset
bool isInternalCmd(char *cmd, size_t cmdLength) {
    static char *internalCmd[19] = {
            "bg", "cd", "continus", "echo", "exec", "exit", "fg", "jobs", "pwd", "set", "test", "time",
            "umask", "unset", "clr", "dir", "environ", "help", "quit"
    };
    char arg[MAXLENGTH];
    size_t size = getFirstArg(cmd, cmdLength, arg);
    for (int i = 0; i < 19; i++) {
        if (strcmp(internalCmd[i], arg) == 0) {
            return true;
        }
    }

    return false;
}

int isIoRedirect(char *str, size_t strLength) {
    for(ssize_t i = 0;i<strLength; i++){
        if(str[i] == '<' || str[i] == '>' )
            return i;
    }
    return -1;
}

bool isPipeCharacter(char cc){
    return cc == '|';
}

ssize_t getAllPipeIndex(char *cmd, size_t cmdLength, int *index[], size_t maxIndex){
    return findCharacter(cmd, cmdLength, index, maxIndex, isPipeCharacter);
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