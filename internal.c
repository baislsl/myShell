//
// Created by baislsl on 17-7-23.
//
#include <dirent.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "internal.h"
#include "pid.h"
#include "myshell.h"
#include "utility.h"

#define MSG_LENGTH 1024

int _pwd(const char **argv, size_t argc) {
    if (argc != 1) {
        err_sys("gg pwd\n");
        return -1;
    }
    write(STDOUT_FILENO, getPath(), MAX_PATH);
    write(STDOUT_FILENO, "\n", 2);
    return 0;
}

int _cd(const char **argv, size_t argc) {
    if (argc == 1) {
        return chdir(getenv("HOME"));
    }
    if (argc != 2) {
        err_sys("argument for cd must be 2\n");
        return -1;
    }
    if (chdir(argv[1]) < 0) {
        err_sys("No such path\n");
        _pwd(NULL, 1);
        return -1;
    }
    return 0;

}


int _time(const char **argv, size_t argc) {
    time_t tp = time(NULL);
    fprintf(stdout, "%s", ctime(&tp));
}

int _clr(const char **argv, size_t argc) {
    static char output[] = "\033[1A\033[2J\033[H";
    if (write(STDOUT_FILENO, output, strlen(output)) == -1)
        return -1;
    else
        return 0;
}

int dirPath(const char *path) {
    DIR *dir;
    if ((dir = opendir(path)) == NULL) {
        char msg[MSG_LENGTH];
        sprintf(msg, "Can not open directory : %s\n", path);
        err_sys(msg);
        return -1;
    }
    struct dirent *file;
    while ((file = readdir(dir)) != NULL) {
        write(STDOUT_FILENO, file->d_name, strlen(file->d_name));
        write(STDOUT_FILENO, " ", 1);
    }
    write(STDOUT_FILENO, "\n", 1);
}

/**
 * list arg path
 * if no, list current path
 * do not support "-x" yet
  * */
int _dir(const char **argv, size_t argc) {
    if (argc == 1) { // list current path
        return dirPath(getPath());
    }
    char info[MSG_LENGTH];
    for (int i = 1; i < argc; i++) {
        if (argc != 2) {
            sprintf(info, "%s:\n", argv[i]);
            write(STDOUT_FILENO, info, strlen(info));
        }
        dirPath(argv[i]);
    }
}


int _echo(const char **argv, size_t argc) {
    do {
        --argc;
        ++argv;
        if (*argv[0] == '$') {
            char *value = getenv(*argv + 1);
            if(value != NULL){
                fputs(value, stdout);
            }
        } else {
            fputs(*argv, stdout);
        }
        if (argc > 1) {
            fputc(' ', stdout);
        } else {
            fputc('\n', stdout);
        }
    } while (argc > 1);

}

int _help(const char **argv, size_t argc) {

}

int _quit(const char **argv, size_t argc) {
    exit(EXIT_SUCCESS);
}

int _umask(const char **argv, size_t argc) {
    if (argc == 1) {
        mode_t mode = umask(0);
        fprintf(stdout, "%03o\n", mode);
        umask(mode);
    } else {
        // cast the second arg to octal integer
        mode_t mode = (mode_t) strtoul(argv[1], NULL, 8);
        umask(mode);
    }

}

int _jobs(const char **argv, size_t argc) {
    showAllPid();
}

typedef int bgFgFunc(size_t);

int bgAndFg(const char **argv, size_t argc, bgFgFunc func) {
    if (argc == 1) {
        ssize_t pidNumber = getLastPidNumber();
        if (pidNumber >= 0)
            func((size_t) pidNumber);
    } else {
        while (argc > 1) {
            --argc;
            ++argv;
            unsigned long pidNumber = strtoul(*argv, NULL, 10);
            func(pidNumber);
        }
        return 0;
    }
}

int _bg(const char **argv, size_t argc) {
    return bgAndFg(argv, argc, makeBackground);
}

int _fg(const char **argv, size_t argc) {
    return bgAndFg(argv, argc, makeForeground);
}

int _exec(const char **argv, size_t argc) {
    char cmd[MAX_LENGTH];
    do {
        ++argv;
        --argc;
        strcat(cmd, *argv);
        strcat(cmd, " ");
    } while ((argc > 1));
    runCommand(cmd);
    exit(0);
}

int _export(const char **argv, size_t argc) {
    int ret = 0;
    do {
        ++argv;
        --argc;
        if (putenv((char *) *argv) != 0)
            ret = -1;
    } while ((argc > 1));
    return ret;
}

// global environ definition
/**
 * The variable environ points to an array of pointers to strings called
       the "environment".  The last pointer in this array has the value
       NULL.  (This variable must be declared in the user program, but is
       declared in the header file <unistd.h> if the _GNU_SOURCE feature
       test macro is defined.)  This array of strings is made available to
       the process by the exec(3) call that started the process.  When a
       child process is created via fork(2), it inherits a copy of its
       parent's environment.

 * */
extern char **environ;

int _set(const char **argv, size_t argc) {
    for (size_t i = 0; environ[i] != NULL; i++) {
        fprintf(stdout, "%s\n", environ[i]);
    }
    return 0;
}

int _unset(const char **argv, size_t argc) {
    int ret = 0;
    do {
        ++argv;
        --argc;
        if (unsetenv(*argv) == -1)
            ret = -1;
    } while ((argc > 1));
    return ret;
}

int _environ(const char **argv, size_t argc) {
    _set(argv, argc);
}

static internalFuncTie internalFuncList[] = {
        {_cd,      "cd"},
        {_pwd,     "pwd"},
        {_clr,     "clr"},
        {_dir,     "dir"},
        {_environ, "environ"},
        {_echo,    "echo"},
        {_help,    "help"},
        {_quit,    "quit"},
        {_time,    "time"},
        {_umask,   "umask"},
        {_jobs,    "jobs"},
        {_bg,      "bg"},
        {_fg,      "fg"},
        {_exec,    "exec"},
        {_export,  "export"},
        {_set,     "set"},
        {_unset,   "unset"}
};

bool isInternalCmd(char *name, size_t length) {
    for (int i = 0; i < sizeof(internalFuncList) / sizeof(internalFuncTie); i++) {
        if (strncmp(internalFuncList[i].cmd, name, length) == 0)
            return true;
    }
    return false;
}

int execInner(char *name, const char **argv, size_t argc) {
    for (int i = 0; i < sizeof(internalFuncList) / sizeof(internalFuncTie); i++) {
        internalFuncTie *icmd = internalFuncList + i;
        if (strcmp(icmd->cmd, name) == 0) {
            return (icmd->func)(argv, argc);
        }
    }
    return -1;
}

