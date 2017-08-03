//
// Created by baislsl on 17-7-22.
//

#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <fcntl.h>
#include "internal.h"
#include "command.h"
#include "parser.h"

#include <stdio.h>

extern char** environ;

int main(int argc, char* argv[])
{
    struct stat buf;
    printf("1: %d\n", stat("./pid.c"), &buf);
    printf("1: %d\n", stat("./pid."), &buf);

}
