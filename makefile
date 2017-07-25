all : utility.o command.o parser.o internal.o myshell.o
	cc -o shell utility.o command.o parser.o internal.o myshell.o
utility.o : utility.c utility.h parser.o
command.o: command.c command.h myshell.h utility.h parser.h internal.h
parser.o: parser.c parser.h
internal.o: internal.c internal.h utility.h myshell.h
myshell.o: myshell.c internal.h utility.h myshell.h command.h parser.h
clean : 
	rm *.o
