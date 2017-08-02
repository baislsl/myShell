//
// Created by baislsl on 17-7-23.
//
#include <dirent.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include "internal.h"
#include "pid.h"

#define MSG_LENGTH 1024

int _pwd(const char **argv, size_t argc) {
    if (argc != 1) {
        err_sys("gg pwd\n", STDERR_FILENO);
        return -1;
    }
    char path[MAXPATH];
    if (getcwd(path, MAXPATH) == NULL) {
        err_sys("get path fail", STDERR_FILENO);
    }
    write(STDOUT_FILENO, path, strlen(path));
    write(STDOUT_FILENO, "\n", 2);
    return 0;
}

int _cd(const char **argv, size_t argc) {
    if (argc == 1) {
        return chdir(getenv("HOME"));
    }
    if (argc != 2) {
        err_sys("argument for cd must be 2\n", STDERR_FILENO);
        return -1;
    } else {
        if (chdir(argv[1]) < 0) {
            err_sys("No such path\n", STDERR_FILENO);
            _pwd(NULL, 1);
            return -1;
        }
        return 0;
    }
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
        err_sys(msg, STDERR_FILENO);
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

int _environ(const char **argv, size_t argc) {
    puts(getenv("PATH"));
}

int _echo(const char **argv, size_t argc) {
    --argc;
    ++argv;
    while (argc > 0) {
        fputs(argv[0], stdout);
        argc--;
        argv++;
        if (argc > 0)
            putchar(' ');
    }
    fputs("\n", stdout);

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

int _bg(const char **argv, size_t argc) {
    if (argc == 1) {
        ssize_t pidNumber = getLastPidNumber();
        if(pidNumber >= 0)
            makeBackground((size_t)pidNumber);
    } else{
        while(argc > 1){
            --argc;
            ++argv;
            unsigned long pidNumber = strtoul(*argv, NULL, 10);
            makeBackground(pidNumber);
        }
        return 0;
    }
}

int _fg(const char **argv, size_t argc) {
    if (argc == 1) {
        ssize_t pidNumber = getLastPidNumber();
        if(pidNumber >= 0)
            makeBackground((size_t)pidNumber);    } else{
        while(argc > 1){
            --argc;
            ++argv;
            unsigned long pidNumber = strtoul(*argv, NULL, 10);
            makeForeground(pidNumber);
        }
        return 0;
    }
}

int _exec(const char **argv, size_t argc){

    exit(0);
}


typedef int innerFunc(const char **argv, size_t n);

struct tie {
    innerFunc *func;
    char *cmd;
};
typedef struct tie Tie;
static Tie innerFuncList[] = {
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
        {_exec, "exec"}
};

bool isInternalCmd(char *name, size_t length) {
    for (int i = 0; i < sizeof(innerFuncList) / sizeof(Tie); i++) {
        if (strncmp(innerFuncList[i].cmd, name, length) == 0)
            return true;
    }
    return false;
}

int execInner(char *name, const char **argv, size_t argc) {
    for (int i = 0; i < sizeof(innerFuncList) / sizeof(Tie); i++) {
        Tie *icmd = innerFuncList + i;
        if (strcmp(icmd->cmd, name) == 0) {
            return (icmd->func)(argv, argc);
        }
    }
    return -1;
}

