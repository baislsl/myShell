//
// Created by baislsl on 17-7-25.
//

#ifndef MYSHELL_PID_H
#define MYSHELL_PID_H
#include <fcntl.h>
#include <stddef.h>
#include "command.h"

#define MAX_PID 256

enum Condition{
    STOP, FINISH, BACKGROUND, CONTINUE
};

struct pidNode{
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

int getPidNumber(pid_t pid);

int removePid(pid_t pid);

int checkProcess();

ssize_t getLastPidNumber();

#endif //MYSHELL_PID_H
