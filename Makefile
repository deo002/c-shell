output: shell.o tokenize.o commands.o parse.o builtin.o execute_command.o signal_handlers.o prompt.o
	gcc -g shell.o tokenize.o commands.o parse.o builtin.o execute_command.o signal_handlers.o prompt.o -o exe

shell.o: shell.c
	gcc -g -c shell.c

tokenize.o: tokenize.c
	gcc -g -c tokenize.c

commands.o: commands.c
	gcc -g -c commands.c

parse.o: parse.c
	gcc -g -c parse.c

builtin.o: builtin.c
	gcc -g -c builtin.c

prompt.o: prompt.c
	gcc -g -c prompt.c

signal_handlers.o: signal_handlers.c
	gcc -g -c signal_handlers.c

execute_command.o: execute_command.c
	gcc -g -c execute_command.c

clean:
	rm *.o exe
