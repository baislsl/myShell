//
// Created by baislsl on 17-7-25.
//
#include <wait.h>
#include <stdlib.h>
#include "pid.h"



pNode plist[MAX_PID];
size_t count;
extern int forePid;

ssize_t findPidNodeWithPidNumber(size_t number) {
    for (size_t i = 0; i < count; i++) {
        if (plist[i].number == number)
            return i;
    }
    return -1;
}

ssize_t findPidNodeWithPid(pid_t pid) {
    for (size_t i = 0; i < count; i++) {
        if (plist[i].pid == pid)
            return i;
    }
    return -1;
}

void printPidNode(pNode *p) {
    fprintf(stdout, "[%lu] %d   ", p->number, p->pid);
    switch (p->condition) {
        case STOP:
            fputs("STOP", stdout);
            break;
        case CONTINUE:
            fputs("CONTINUE", stdout);
            break;
        case FINISH:
            fputs("FINISH", stdout);
            break;
        case BACKGROUND:
            fputs("BACKGROUND", stdout);
            break;
    }
    fputs("    ", stdout);
    printCommand(p->cmd);
}

void showAllPid() {
    for (int i = 0; i < count; i++) {
        printPidNode(&plist[i]);
    }
}

int removePidList(size_t index) {
    plist[index].condition = FINISH;
//    freeCommand(plist[index].cmd);
//    free(plist[index].cmd);
}

int makeBackground(size_t pidNumber) {
    ssize_t i = findPidNodeWithPidNumber(pidNumber);
    if (i < 0)
        return 1;
    plist[i].condition = CONTINUE;
    printPidNode(&plist[i]);
    kill(plist[i].pid, SIGCONT);
}

int makeForeground(size_t pidNumber) {
    int status;
    ssize_t i = findPidNodeWithPidNumber(pidNumber);
    if (i < 0)
        return 1;

    plist[i].condition = CONTINUE;
    printPidNode(&plist[i]);
    kill(plist[i].pid, SIGCONT);
    forePid = plist[i].pid;
    waitpid(plist[i].pid, &status, WUNTRACED);
    if (!WIFSTOPPED(status)) {
        removePidList((size_t) i);
    }
    forePid = -1;
}

int addPid(pid_t pid, CommandPtr cmd, enum Condition condition) {
    plist[count].pid = pid;
    plist[count].cmd = (CommandPtr) malloc(sizeof(Command));
    commandCopy(plist[count].cmd, cmd);
    plist[count].condition = condition;
    plist[count].number = count + 1;
    ++count;
}

int getPidNumber(pid_t pid) {
    return (int) findPidNodeWithPid(pid);
}

int removePid(pid_t pid) {
    ssize_t i = findPidNodeWithPid(pid);
    if (i < 0) {
        return 1;
    } else {
        removePidList((size_t) i);
        return 0;
    }

}

int testAll() {
    int status;
    for (int i = 0; i < count; i++) {
        if (plist[i].condition == FINISH) continue;
        int r = waitpid(plist[i].pid, &status, WNOHANG);
        if (r != 0) {
            plist[i].condition = FINISH;
            if (WIFEXITED(status)) {
                fputs("normal exit", stdout);
                printPidNode(&plist[i]);
                removePid(i);
            } else if (WIFSIGNALED(status)) {
                fputs("abnormal exit", stdout);
                printPidNode(&plist[i]);
                removePid(plist[i].pid);
            }
        }
    }
}

ssize_t getLastPidNumber() {
    for (size_t i = count - 1; i >= 0; i--) {
        if (plist[i].condition != FINISH) {
            return i;
        }
    }
    return -1;
}