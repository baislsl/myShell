#include <stdio.h>
#include <unistd.h>

#define MAXLINE 80 /* The maximum length command */

void init();

/*设置搜索路径*/
void setpath(char *newPath);

/*读取用户输入*/
int readCommand();

/*解析内部命令*/
int isInternalCmd(char *cmd, int cmdLength);

/*解析管道命令*/
int isPipe(char *cmd, int cmdLength);

/*解析重定向*/
int isIoRedirect(char *cmd, int cmdLength);

int normalCmd(char *cmd, int cmdLength, int inFd, int outFd, int fork);

/*其他函数....... */
void init() {
/*执行普通命令*//*设置命令提示符*/
    printf("myshell>");
    fflush(stdout);
/*设置默认的搜索路径*/
    setpath("/bin:/usr / bin");
/*......*/
}




int main(void) {
    char *args[MAXLINE / 2 + 1]; /* command line arguments */
    char line[MAXLINE];
    int shouldRun = 1; /* flag to determine when to exit program */
    init();

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