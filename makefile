OBJECTS = utility.o command.o parser.o internal.o myshell.o pid.o forePid.o param.o
CC = gcc
all : ${OBJECTS}
	$(CC) -o myshell $(OBJECTS)
clean :
	rm *.o
    
