//
// Created by baislsl on 17-7-22.
//

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include <wait.h>

int main(){
    char *avg[] = {"ls", "-al"};
    execvp("ls", avg);
    puts("gg");
}