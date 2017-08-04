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

int _pwd(const char **argv, size_t argc) {
    char *path = getPath();
    write(STDOUT_FILENO, path, strlen(path));
    write(STDOUT_FILENO, "\n", 2);
    return 0;
}

int _cd(const char **argv, size_t argc) {
    if (argc == 1) {
        return chdir(getenv("HOME"));
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
    return 0;
}

int _clr(const char **argv, size_t argc) {
    // "\003[2J" clear the screen
    // "\003[H" move the cursor to the upper-left corner of the screen
    static char output[] = "\033[2J\033[H";
    if (write(STDOUT_FILENO, output, strlen(output)) == -1)
        return -1;
    else
        return 0;
}

// list the file under path to stdout
int dirPath(const char *path) {
    DIR *dir;
    if ((dir = opendir(path)) == NULL) { // open directory
        char msg[PATH_MAX];
        sprintf(msg, "Can not open directory : %s\n", path);
        err_sys(msg);
        return -1;
    }
    struct dirent *file;
    // scan for the file in directory
    while ((file = readdir(dir)) != NULL) {
        write(STDOUT_FILENO, file->d_name, strlen(file->d_name));
        write(STDOUT_FILENO, " ", 1);
    }
    write(STDOUT_FILENO, "\n", 1);
}

int _dir(const char **argv, size_t argc) {
    if (argc == 1) { // list current path
        return dirPath(getPath());
    }
    char info[PATH_MAX];
    for (int i = 1; i < argc; i++) {
        if (argc != 2) {
            sprintf(info, "%s:\n", argv[i]);
            write(STDOUT_FILENO, info, strlen(info));
        }
        dirPath(argv[i]);
    }
}

/**
 * handle input string of $?, $#, $*, $@, $1, $2, ... , $PATH, $HOME ...
 * if the argv do not contain $XXX, directly copy to dest
 * else analysis the character after '$'
 * */
void translate(const char *src, char *dest) {
    if (*src == '$') {
        if (strlen(src) == 2) {
            if (src[1] == '?') {                            // $?
                sprintf(dest, "%d", getExitState());
            } else if (src[1] >= '0' && src[1] <= '9') {    // $1, $2 ... $9
                strcpy(dest, param((size_t) (src[1] - '0')));
            } else if (src[1] == '@' || src[1] == '*') {    // $@, $*
                printAllParam(dest);
            } else if (src[1] == '#') {                     // $#
                sprintf(dest, "%d", getArgc());
            } else {                                        // $xxx
                char *env = getenv(src + 1);
                if (env == NULL)
                    dest[0] = 0;
                else
                    strcpy(dest, getenv(src + 1));
            }
        } else {
            char *env = getenv(src + 1);
            if (env == NULL)
                dest[0] = 0;
            else
                strcpy(dest, getenv(src + 1));
        }
    } else {
        strcpy(dest, src);
    }
}

int _echo(const char **argv, size_t argc) {
    char dest[MAX_LENGTH];
    do {
        --argc;
        ++argv;
        translate(*argv, dest); // handle '$' transfer
        if (dest != NULL && dest[0])
            fputs(dest, stdout);
        fputs("\n", stdout);
    } while (argc > 1);
    return 0;
}

int _help(const char **argv, size_t argc) {
    char path[MAX_LENGTH];
    char line[MAX_LENGTH];
    int ret = 0;
    do {
        --argc;
        ++argv;
        sprintf(path, "./doc/%s", *argv);
        FILE *fp = fopen(path, "r");
        if (fp == NULL) {
            sprintf(path, "没有关于%s指令的帮助\n", *argv);
            err_sys(path);
            ret = -1;
            continue;
        }
        while (fgets(line, MAX_LENGTH, fp) != NULL) {
            fputs(line, stdout);
        }
    } while (argc > 1);
    return ret;
}

int _quit(const char **argv, size_t argc) {
    if (argc > 1) {
        exit((int) strtoul(argv[1], NULL, 10));
    }
    exit(getExitState());
}

int _umask(const char **argv, size_t argc) {
    if (argc == 1) {
        fprintf(stdout, "%03o\n", getUmask());
    } else {
        // cast the second arg to octal integer
        mode_t mode = (mode_t) strtoul(argv[1], NULL, 8);
        umask(mode);
    }
    return 0;
}

int _jobs(const char **argv, size_t argc) {
    showAllPid();
    return 0;
}

typedef int bgFgFunc(size_t);

/**
 * handle command fg and bg
 *
 * @param func the function that put a process in foreground or background
 *      makeBackground when called by bg
 *      makeForeground when called by fg
 * */
int bgAndFg(const char **argv, size_t argc, bgFgFunc func) {
    if (argc == 1) {
        ssize_t pidNumber = getLastPidNumber();
        if (pidNumber >= 0)
            return func((size_t) pidNumber);
        return -1;
    } else {
        while (argc > 1) {
            --argc;
            ++argv;
            unsigned long pidNumber = strtoul(*argv, NULL, 10);
            if (func(pidNumber) < 0)
                return -1;
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
    } while (argc > 1);
    runCommand(cmd);
    exit(0);
}

/**
 * Variable environ is declared in unistd.h
 * It points to an array of pointers to strings of environment parameters,
 * and the last pointer of environ is NULL.
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
    } while (argc > 1);
    return ret;
}

int _export(const char **argv, size_t argc) {
    int ret = 0;
    do {
        ++argv;
        --argc;

        // must malloc environment in stack and do not free it
        // putenv only add environment to global array environ
        char *environment = (char *) malloc(strlen(*argv) * sizeof(char));
        strcpy(environment, *argv);
        if (putenv(environment) != 0) {
            free(environment);
            ret = -1;
        }
    } while (argc > 1);
    return ret;
}

int _environ(const char **argv, size_t argc) {
    _set(argv, argc);
}

int _continue(const char **argv, size_t argc) {
    return 1;
}

int _shift(const char **argv, size_t argc) {
    size_t i = 1;
    if (argc > 1) {
        i = strtoul(argv[1], NULL, 10);
    }
    return shift(i);
}

/**
 * called by function _test to handle 2 argument in test
 * judge following forms:
 *      -z STRING      True if string is empty.
 *      -n STRING      True if string is not empty.
 *      -b FILE        True if file is block special.
 *      -c FILE        True if file is character special.
 *      -d FILE        True if file is a directory.
 *      -e FILE        True if file exists.
 *      -f FILE        True if file exists and is a regular file.
 *      -h FILE        True if file is a symbolic link.
 *      -L FILE        True if file is a symbolic link.
 *      -k FILE        True if file has its `sticky' bit set.
 *      -p FILE        True if file is a named pipe.
 *      -r FILE        True if file is readable by you.
 *      -s FILE        True if file exists and is not empty.
 *      -S FILE        True if file is a socket.
 *      -w FILE        True if the file is writable by you.
 *      -x FILE        True if the file is executable by you.
 * @return 1 for true, 0 for false, -1 for input format error
 * */
int test2(const char *argv1, const char *argv2) {
    char a1[MAX_LENGTH], a2[MAX_LENGTH];
    translate(argv1, a1);
    translate(argv2, a2);
    if (a1[0] != '-' || strlen(a1) != 2)
        return -1;
    char cc = a1[1];
    if (cc == 'z')  // True if string is empty.
        return strlen(a1) == 0;
    if (cc == 'n')  // True if string is not empty.
        return strlen(a1) != 0;

    struct stat buf;
    if (stat(a2, &buf) == -1) { // file do not exist
        return 0;
    }
    switch (cc) {
        case 'b':   // True if file is block special.
            return S_ISBLK(buf.st_mode);
        case 'c':   // True if file is character special.
            return S_ISCHR(buf.st_mode);
        case 'd':   // True if file is a directory.
            return S_ISDIR(buf.st_mode);
        case 'e':   // True if file exists.
            return 1;
        case 'f':   // True if file exists and is a regular file.
            return S_ISREG(buf.st_mode);
        case 'h':   // True if file is a symbolic link.
        case 'L':
            return S_ISLNK(buf.st_mode);
        case 'k':   // True if file has its `sticky' bit set.
            return buf.st_mode & S_ISVTX;
        case 'p':   // True if file is a named pipe.
            return S_ISFIFO(buf.st_mode);
        case 'r':   // True if file is readable by user
            return buf.st_mode & S_IRUSR;
        case 's':   // True if file is readable by you.
            return buf.st_size > 0;
        case 'S':   // True if file is a socket.
            return S_ISSOCK(buf.st_mode);
        case 'w':   // True if FD is opened on a terminal.
            return buf.st_mode & S_IWUSR;
        case 'x':   // True if FD is opened on a terminal.
            return buf.st_mode & S_IXUSR;
        default:
            return -1;
    }
}

/**
 * called by function _test to handle 3 argument in test
 * judge following forms:
 *      FILE1 -nt FILE2     True if file1 is newer than file2 (according to modification date).
 *      FILE1 -ot FILE2     True if file1 is older than file2.
 *      STRING1 = STRING2   True if the strings are equal.
 *      STRING1 != STRING2  True if the strings are not equal.
 *      STRING1 <= STRING2  True if STRING1 sorts before or equal STRING2 lexicographically
 *      STRING1 >= STRING2   True if STRING1 sorts after or equal STRING2 lexicographically.
 * for consideration of simplicity, the program do not support "<" and ">" comparison,
 * because it will be mistaken as IO redirect
 *
 * @return 1 for true, 0 for false, -1 for input format error
 * */
int test3(const char *argv1, const char *argv2, const char *argv3) {
    char a1[MAX_LENGTH], a2[MAX_LENGTH], a3[MAX_LENGTH];
    translate(argv1, a1);
    strcpy(a2, argv2);
    translate(argv3, a3);
    int cmp = strcmp(a1, a3);
    if (strcmp(a2, "=") == 0) {
        return cmp == 0;
    } else if (strcmp(a2, "!=") == 0) {
        return cmp != 0;
    } else if (strcmp(a2, "<=") == 0) {
        return cmp <= 0;
    } else if (strcmp(a2, ">=") == 0) {
        return cmp >= 0;
    } else if (strcmp(a2, "-nt") == 0) {    // True if a1 is newer than a3
        struct stat buf1, buf2;
        if (stat(a1, &buf1) == -1)
            return -1;
        if (stat(a3, &buf2) == -1) {
            return -1;
        }
        return (buf1.st_mtim.tv_sec > buf2.st_mtim.tv_sec)
               || ((buf1.st_mtim.tv_sec == buf2.st_mtim.tv_sec) && (buf1.st_mtim.tv_nsec > buf2.st_mtim.tv_nsec));
    } else if (strcmp(a2, "-ot") == 0) {    // True if a1 is older than a3
        struct stat buf1, buf2;
        if (stat(a1, &buf1) == -1)
            return -1;
        if (stat(a3, &buf2) == -1) {
            return -1;
        }
        return (buf1.st_mtim.tv_sec < buf2.st_mtim.tv_sec)
               || ((buf1.st_mtim.tv_sec == buf2.st_mtim.tv_sec) && (buf1.st_mtim.tv_nsec < buf2.st_mtim.tv_nsec));
    } else {
        return -1;
    }
}

int _test(const char **argv, size_t argc) {
    enum {
        NONE, AND, OR
    };
    static char *and = "-a";
    static char *or = "-o";
    int result = 0, relation = NONE;
    size_t cur = 1;
    while (1) {
        size_t begin = cur, end = cur;

        // search for "-a" and "-o"
        while (end < argc && strcmp(and, argv[end]) != 0 && strcmp(or, argv[end]) != 0) {
            ++end;
        }

        // get the result of each sentence split by "-a" / "-o"
        int ret;
        if (end - begin == 2) {         // 2 parameter
            ret = test2(argv[begin], argv[begin + 1]);
        } else if (end - begin == 3) {  // 3 parameter
            ret = test3(argv[begin], argv[begin + 1], argv[begin + 2]);
        } else {
            return 0;
        }
        if (ret == -1)   // format error
            return 0;

        if (relation == AND) {
            result &= ret;
        } else if (relation == OR) {
            result |= ret;
        } else {    // relation = NONE, the first sentence
            result = ret;
        }
        if (end == argc) return result;

        // update relation
        relation = (strcmp(and, argv[end]) == 0) ? AND : OR;

        cur = end + 1;
    }
}

int __exit(const char **argv, size_t argc) {
    return _quit(argv, argc);
}

/**
 * this variable store all the internal command name
 * and their according executing function
 *
 * each internal command, its executing function is
 * defined as "int _commandName(const char **argv, size_t argc)"
 * note that _exit has been defined in unistd.h, the executing function of
 * exit is named "__exit"
 * */
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
        {_test,     "test"},
        {__exit,    "exit"}
};

// judge whether a given command name is a internal command
bool isInternalCmd(char *name, size_t length) {
    for (int i = 0; i < sizeof(internalFuncList) / sizeof(internalFuncTie); i++) {
        if (strncmp(internalFuncList[i].cmd, name, length) == 0)
            return true;
    }
    return false;
}

/**
 * run internal command
 *
 * @return return -1 if the command is not a internal command
 * else return the result of the internal command
 * */
int execInternal(const char **argv, size_t argc) {
    for (int i = 0; i < sizeof(internalFuncList) / sizeof(internalFuncTie); i++) {
        internalFuncTie *icmd = internalFuncList + i;
        if (strcmp(icmd->cmd, argv[0]) == 0) {
            return (icmd->func)(argv, argc);
        }
    }
    return -1;
}

