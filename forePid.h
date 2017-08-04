//
// Created by baislsl on 17-8-3.
//

#ifndef MYSHELL_FOREPID_H
#define MYSHELL_FOREPID_H

#include <fcntl.h>

int foregroundWait(pid_t pid);

void sig_tstp(int signal);

void sig_quit(int signal);

void sig_int(int signal);

#endif //MYSHELL_FOREPID_H
