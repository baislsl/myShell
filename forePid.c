//
// Created by baislsl on 17-8-3.
//

#include <wait.h>
#include "forePid.h"
#include "utility.h"

pid_t forePid = -1;

int foreGroundWait(pid_t pid){
    int status;
    forePid = pid;
    if (waitpid(pid, &status, WUNTRACED) != pid) {
        err_sys("wait pid error");
    }
    forePid = -1;
    return status;
}

int getForePid(){
    return forePid;
}
