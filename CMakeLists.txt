cmake_minimum_required(VERSION 3.7)
project(myShell)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES myshell.c myshell.h utility.c internal.h utility.h command.c command.h internal.c parser.c parser.h)
add_executable(myShell ${SOURCE_FILES})