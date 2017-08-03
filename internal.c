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
#include "param.h"

#define MSG_LENGTH 1024

int _pwd(const char **argv, size_t argc) {
    if (argc != 1) {
        err_sys("gg pwd\n");
        return -1;
    }
    char *path = getPath();
    write(STDOUT_FILENO, path, strlen(path));
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

void translate(const char *argv, char *dest) {
    if (*argv == '$') {
        if (strlen(argv) == 2) {
            if (argv[1] == '?') {
                sprintf(dest, "%d", getExitState());
            } else if (argv[1] >= '0' && argv[1] <= '9') {
                strcpy(dest, param((size_t) (argv[1] - '0')));
            } else if (argv[1] == '@' || argv[1] == '*') {
                printAllParam(dest);
            } else {
                strcpy(dest, getenv(argv + 1));
            }
        } else {
            strcpy(dest, getenv(argv + 1));
        }
    } else {
        strcpy(dest, argv);
    }
}


int _echo(const char **argv, size_t argc) {
    char dest[MAX_LENGTH];
    do {
        --argc;
        ++argv;
        translate(*argv, dest);
        if (dest != NULL && dest[0])
            fputs(dest, stdout);
        fputs("\n", stdout);
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


int _export(const char **argv, size_t argc) {
    /**
     * 这里一定要将 environment 定义为static变量,
     * 因为执行putenv栈的内存会发生改变,
     * 如果没有static关键词, 会发生错误
     * */
    static char environment[MAX_LENGTH];
    int ret = 0;
    do {
        ++argv;
        --argc;
        strcpy(environment, *argv);
        if (putenv(environment) != 0) {
            ret = -1;
        }
    } while (argc > 1);
    return ret;
}

int _environ(const char **argv, size_t argc) {
    _set(argv, argc);
}

int _continue(const char **argv, size_t argc) {
    return 0;
}

int _shift(const char **argv, size_t argc) {
    size_t i = 1;
    if (argc > 1) {
        i = strtoul(argv[1], NULL, 10);
    }
    shift(i);
}

// -? FILE
int test2(const char *argv1, const char *argv2) {
    char a1[MAX_LENGTH], a2[MAX_LENGTH];
    translate(argv1, a1);
    translate(argv2, a2);
    if (a1[0] != '-' || strlen(a1) != 2)
        return -1;
    char *all = "abcdefghLkprsStuwxOGNzn";
    char cc = a1[1];
    size_t i;
    for (i = 0; i < strlen(all); i++) {
        if (all[i] == cc) break;
    }
    if (i == strlen(all))
        return -1;

    if (cc == 'z') {
        return strlen(a1) == 0;
    } else if (cc == 'n') {
        return strlen(a1) != 0;
    } else {
        struct stat buf;
        if (stat(a2, &buf) == -1) {
            if (cc == 'a' || cc == 'e')
                return 1;
            return 0;
        }
        switch (cc) {
            case 'a':
            case 'e':
                return 0;
            case 'b':
                return S_ISBLK(buf.st_mode);
            case 'c':
                return S_ISCHR(buf.st_mode);
            case 'd':
                return S_ISDIR(buf.st_mode);
            case 'f':
                return S_ISREG(buf.st_mode);
            case 'h':
            case 'L':
                return S_ISLNK(buf.st_mode);
            case 's':
                return S_ISSOCK(buf.st_mode);
            default:
                return -1;
        }
    }


}

int test3(const char *argv1, const char *argv2, const char *argv3) {
    char a1[MAX_LENGTH], a2[MAX_LENGTH], a3[MAX_LENGTH];
    translate(argv1, a1);
    translate(argv2, a2);
    translate(argv3, a3);
    int cmp = strcmp(a1, a3);
    if (strcmp(a2, "=") == 0) {
        return cmp == 0;
    } else if (strcmp(a2, "!=") == 0) {
        return cmp != 0;
    } else if (strcmp(a2, "<") == 0) {
        return cmp < 0;
    } else if (strcmp(a2, ">") == 0) {
        return cmp > 0;
    } else if (strcmp(a2, "<=") == 0) {
        return cmp <= 0;
    } else if (strcmp(a2, ">=") == 0) {
        return cmp >= 0;
    } else if (strcmp(a2, "-nt") == 0) {
        struct stat buf1, buf2;
        if (stat(a1, &buf1) == -1)
            return -1;
        if (stat(a3, &buf2) == -1) {
            return -1;
        }
        return (buf1.st_mtim.tv_sec > buf2.st_mtim.tv_sec)
               || ((buf1.st_mtim.tv_sec == buf2.st_mtim.tv_sec) && (buf1.st_mtim.tv_nsec > buf2.st_mtim.tv_nsec));
    } else if (strcmp(a2, "-ot") == 0) {
        struct stat buf1, buf2;
        if (stat(a1, &buf1) == -1)
            return -1;
        if (stat(a3, &buf2) == -1) {
            return -1;
        }
        return (buf1.st_mtim.tv_sec < buf2.st_mtim.tv_sec)
               || ((buf1.st_mtim.tv_sec == buf2.st_mtim.tv_sec) && (buf1.st_mtim.tv_nsec < buf2.st_mtim.tv_nsec));
    } else if (strcmp(a2, "ef")) {

        return 0;
    } else {
        return -1;
    }
}

// help test
// -a -o
// FILE
// STRING

enum {
    NONE, AND, OR
};

int _test(const char **argv, size_t argc) {
    static char *and = "-a";
    static char *or = "-o";
    int ret = 0, relation = NONE, tmp;
    size_t cur = 0;
    while (1) {
        size_t begin = cur, end = cur;
        while (end < argc && strcmp(and, argv[end]) != 0 && strcmp(or, argv[end]) != 0) {
            ++end;
        }
        if (begin - end == 2) {   // 2 parameter
            tmp = test2(argv[begin], argv[begin + 1]);
        } else if (begin - end == 3) {
            tmp = test3(argv[begin], argv[begin + 1], argv[begin + 2]);
        } else {
            return 0;
        }
        if (tmp == -1)   //error
            return 0;
        if (relation == NONE) {
            ret = tmp;
            if (end == argc)
                return ret;
            if (strcmp(and, argv[end]) == 0) {
                relation = AND;
            } else {
                relation = OR;
            }
        } else {
            if (end == argc) {
                if (relation == AND) {
                    return ret & tmp;
                } else {
                    return ret | tmp;
                }
            }
            if (strcmp(and, argv[end]) == 0) {
                ret &= tmp;
                if (relation != AND)
                    return 0;
            } else {
                ret |= tmp;
                if (relation != OR)
                    return 0;
            }
        }
        cur = end + 1;
    }
}

static internalFuncTie internalFuncList[] = {
        {_cd,       "cd"},
        {_pwd,      "pwd"},
        {_clr,      "clr"},
        {_dir,      "dir"},
        {_environ,  "environ"},
        {_echo,     "echo"},
        {_help,     "help"},
        {_quit,     "quit"},
        {_time,     "time"},
        {_umask,    "umask"},
        {_jobs,     "jobs"},
        {_bg,       "bg"},
        {_fg,       "fg"},
        {_exec,     "exec"},
        {_export,   "export"},
        {_set,      "set"},
        {_unset,    "unset"},
        {_continue, "continue"},
        {_shift,    "shift"},
        {_test,     "test"}
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

