//
// Created by baislsl on 17-7-25.
//
#include <wait.h>
#include <stdlib.h>
#include "pid.h"
#include "utility.h"
#include "forePid.h"

/**
 * plist store all the information of process that once run or stop in background
 * for consideration of simplicity, plist do not drop the finished process
 * */
pNode plist[MAX_PID];
size_t count;   // variable size of plist

// get the index of plist index with number
ssize_t findPidNodeWithPidNumber(size_t number) {
    for (size_t i = 0; i < count; i++) {
        if (plist[i].number == number)
            return i;
    }
    return -1;
}

// get the index of plist index with pid
ssize_t findPidNodeWithPid(pid_t pid) {
    for (size_t i = 0; i < count; i++) {
        if (plist[i].pid == pid)
            return i;
    }
    return -1;
}

/**
 * print the information of a pidNode to stdout
 * in format: " [number] pid   cmd "
 * */
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

// print all the information of plist to stdout
void showAllPid() {
    for (int i = 0; i < count; i++) {
        printPidNode(&plist[i]);
    }
}

/**
 * for consideration of simplicity, to remove a process from plist,
 * we just mark the finished process as FINISH
 * but not truly remove it from plist
 * */
int removePidList(size_t index) {
    plist[index].condition = FINISH;
}

// put a process indicated by pidNumber to background
int makeBackground(size_t pidNumber) {
    ssize_t i = findPidNodeWithPidNumber(pidNumber);
    if (i < 0)  // not plist found
        return -1;
    plist[i].condition = CONTINUE;
    printPidNode(&plist[i]);
    kill(plist[i].pid, SIGCONT);

    // since the process is put to backgroungd,
    // we do not use waitpid to check the process
    // the checkAll function will call waitpid
    // and recovery the resource of this process if finished
    return 0;
}

// put a process indicated by pidNumber to foreground
int makeForeground(size_t pidNumber) {
    ssize_t i = findPidNodeWithPidNumber(pidNumber);
    if (i < 0)  // not plist found
        return -1;
    plist[i].condition = CONTINUE;
    printPidNode(&plist[i]);
    kill(plist[i].pid, SIGCONT);

    // when call makeForeground, the process must has been run in background before,
    // so the information of this process must has been stored in plist
    // if the process is stopped again, set the condition to STOP
    // and do not need tho add it to plist again
    int status = foreGroundWait(plist[i].pid);
    if (WIFSTOPPED(status)) {
        plist[i].condition = STOP;
    }
    return 0;
}

// add a background or stop process to plist
int addPid(pid_t pid, CommandPtr cmd, enum Condition condition) {
    plist[count].pid = pid;
    plist[count].cmd = (CommandPtr) malloc(sizeof(Command));
    commandCopy(plist[count].cmd, cmd);
    plist[count].condition = condition;
    plist[count].number = count + 1;
    ++count;        // update count
}

/**
 * when each command was inputted in the shell,
 * this function will be called to check all the condition of the unfinished process in plist
 * this function is essential to avoid defunct process.
 * Once a process condition change to FINISH, print out the information of this process to stdout,
 * this function is called before each command to avoid redirect of the command,
 * ensuring the information is printed to true stdout
 * */
void checkProcess() {
    int status;
    for (size_t i = 0; i < count; i++) {
        if (plist[i].condition == FINISH) continue;

        // for waitpid, use WNOHANG because we just check the condition of process
        // we just need to know whether the process is already dead,
        // if dead, recovery the resources
        int r = waitpid(plist[i].pid, &status, WNOHANG);
        if (r != 0) {
            plist[i].condition = FINISH;
            // check the condition of exit, normal or abnormal
            if (WIFEXITED(status)) {            // normal exit
                fputs("normal exit:", stdout);
                printPidNode(&plist[i]);
                removePidList(i);
            } else if (WIFSIGNALED(status)) {   // abnormal exit
                err_sys("abnormal exit");
                printPidNode(&plist[i]);
                removePidList(i);
            }
        }
    }
}

/**
 * get the last unfinished pid that was put to plist
 * this function is used in command "bg" and "fg" with no parameter
 *
 * @return the pid number of most recent unfinished process in plist
 *  return -1 if not such process exist
 * */
ssize_t getLastPidNumber() {
    for (size_t i = count - 1; i >= 0; i--) {
        if (plist[i].condition != FINISH) {
            return plist[i].number;
        }
    }
    return -1;
}
