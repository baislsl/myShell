all : utility.o command.o parser.o internal.o myshell.o pid.o forePid.o param.o
	cc -o shell utility.o command.o parser.o internal.o myshell.o pid.o forePid.o param.o
clean :
	rm *.o
