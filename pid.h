//
// Created by baislsl on 17-7-25.
//

#ifndef MYSHELL_PID_H
#define MYSHELL_PID_H

#include <fcntl.h>
#include <stddef.h>
#include "command.h"

/**
 * STOP : a process was stopped
 * FINISH : a process has finished
 * BACKGROUND : a process was running in background and never stop before
 * CONTINUE : a process continue after stop
 * */
enum Condition {
    STOP, FINISH, BACKGROUND, CONTINUE
};

// store the information of one process
struct pidNode {
    pid_t pid;
    size_t number;
    CommandPtr cmd;
    enum Condition condition;
};
typedef struct pidNode pNode;

void showAllPid();

int makeBackground(size_t pidNumber);

int makeForeground(size_t pidNumber);

int addPid(pid_t pid, CommandPtr cmd, enum Condition condition);

void checkProcess();

ssize_t getLastPidNumber();

#endif //MYSHELL_PID_H
