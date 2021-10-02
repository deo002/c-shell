output: shell.o tokenize.o parse.o builtin.o execute_command.o signal_handlers.o prompt.o ls.o nightswatch.o history.o user_defined_commands.o
	gcc -g shell.o tokenize.o parse.o builtin.o execute_command.o signal_handlers.o prompt.o ls.o nightswatch.o history.o user_defined_commands.o -o exe

shell.o: shell.c
	gcc -g -c shell.c

ls.o: ls.c
	gcc -g -c ls.c

history.o: history.c
	gcc -g -c history.c

nightswatch.o: nightswatch.c
	gcc -g -c nightswatch.c

tokenize.o: tokenize.c
	gcc -g -c tokenize.c

parse.o: parse.c
	gcc -g -c parse.c

builtin.o: builtin.c
	gcc -g -c builtin.c

prompt.o: prompt.c
	gcc -g -c prompt.c

user_defined_commands.o: user_defined_commands.c
	gcc -g -c user_defined_commands.c

signal_handlers.o: signal_handlers.c
	gcc -g -c signal_handlers.c

execute_command.o: execute_command.c
	gcc -g -c execute_command.c

clean:
	rm *.o exe
