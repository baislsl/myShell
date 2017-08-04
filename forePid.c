//
// Created by baislsl on 17-8-3.
//

#include <wait.h>
#include "forePid.h"
#include "utility.h"

/**
 * forePid store the pid of the process running in the foreground
 * if no process running in forePid, set forePid to -1.
 * note that forePid can be used only in function foreGroundWait and catchSignal,
 * never read or write forePid in other place
 * */
pid_t forePid = -1;

/**
 * wait for process indicated bt pid in the foreground
 * set forePid to pid util the wait for pid process ends
 * the stop, interrupt signal from the process can be caught
 * to stop the wait process
 *
 * @param pid the pid of the process will be put foreground
 * @return the status of wait for pid process
 * */
int foreGroundWait(pid_t pid) {
    int status;
    forePid = pid;  // set forePid before wait

    // in function waitpid, use parameter WUNTRACED allows that
    // the wait process can return when SIGSTOP was caught
    if (waitpid(pid, &status, WUNTRACED) != pid) {
        err_sys("wait pid error");
    }
    forePid = -1;   // reset forePid
    return status;
}

/**
 * catch signal and send it to the process indicated by forePid
 * if forPid is -1, ignore the signal
 * */
void catchSignal(int signal) {
    if (forePid != -1) {
        kill(forePid, signal);
    }
}

// transfer the Ctrl+Z signal from keyboard to SIGSTOP for foreground process
void sig_tstp(int signal) {
    catchSignal(SIGSTOP);
}

// transfer the Ctrl+\ signal from keyboard to SIGQUIT for foreground process
void sig_quit(int signal) {
    catchSignal(signal);
}

// transfer the Ctrl+C signal from keyboard to SIGINT for foreground process
void sig_int(int signal) {
    catchSignal(signal);
}
