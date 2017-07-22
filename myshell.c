#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include <fcntl.h>
#include "myshell.h"

#define MAXLINE 80 /* The maximum length command */
#define MAXPATH 256

char *path[MAXPATH];
size_t pathIndex;

bool isPathDiv(char cc) {
    return cc == ':';
}

int setpath2(char *newPath){
    setenv("PATH", newPath, 1);
    puts(getenv("PATH"));
}

/*设置搜索路径*/
int setpath(char *newPath) {
    char *add[MAXPATH - pathIndex];
    for (int i = 0; i < MAXPATH - pathIndex; i++) {
        add[i] = (char *) malloc(MAXLINE);
    }

    ssize_t n = ridFind(newPath, strlen(newPath), add, MAXPATH - pathIndex, isPathDiv);
    if (n < 0) {
        for(int i=0;i<MAXPATH - pathIndex;i++)
            free(add[i]);
        return -1;
    }
    for (ssize_t i = 0; i < n; i++) {
        path[pathIndex + i] = add[i];
    }
    for (ssize_t i = n; i < MAXPATH - pathIndex; i++) {
        free(add[i]);
    }
    return n;
}

/*读取用户输入*/
int readCommand();

/*解析内部命令*/
int isInternalCmd(char *cmd, int cmdLength);

/*解析管道命令*/
int isPipe(char *cmd, int cmdLength);

/*解析重定向*/
int isIoRedirect(char *cmd, int cmdLength);

/*其他函数....... */
void init() {
/*执行普通命令*//*设置命令提示符*/
    printf("myshell>");
    fflush(stdout);
/*设置默认的搜索路径*/
    setpath2("/bin:/usr/bin");
/*......*/
}

int normalCmd(char *cmd, int cmdLength, int inFd, int outFd, int fork){
    dup2(inFd, STDIN_FILENO);
    dup2(outFd, STDOUT_FILENO);

    char *avg[] = {"ls", "-al"};

    execvp("ls", avg);

    close(inFd);
    close(outFd);


}



int main(void) {
    char *args[MAXLINE / 2 + 1]; /* command line arguments */
    char line[MAXLINE];
    int shouldRun = 1; /* flag to determine when to exit program */
    init();
    int dfout = open("./tmp", O_WRONLY);
    // normalCmd("", 0, STDIN_FILENO, dfout, 0);
    while (shouldRun) {
        read(STDIN_FILENO, line, MAXLINE);



/**
* After reading user input, the steps are:
*内部命令:
*.....
*外部命令:
* (1) fork a child process using fork()
* (2) the child process will invoke execvp()
* (3) if command included &, parent will invoke wait()
*.....
*/
    }
    return 0;
}