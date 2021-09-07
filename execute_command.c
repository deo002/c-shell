#include "headers.h"
#include "execute_command.h"
#include "builtin.h"

int check_builtin_command(char *cmd)
{
    size_t size = sizeof(builtin_commands) / sizeof(char *);

    for (int i = 0; i < (int)size; ++i)
    {
        if (strcmp(cmd, builtin_commands[i]) == 0)
            return i;
    }

    return -1;
}

int execute_command(char **args, int arg_count)
{
    int builtin_command = check_builtin_command(args[0]);

    int is_background = 0;

    if (strcmp(args[arg_count - 1], "&") == 0)
    {
        is_background = 1;
    }

    if (builtin_command == -1)
    {
        free(args[arg_count]);
        args[arg_count] = NULL;
    }

    int pid = fork();

    if (pid == -1)
    {
        perror("fork() error");
        return EXIT_FAILURE;
    }

    if (pid == 0)
    {
        if (builtin_command == -1)
        {
            int res = execvp(args[0], args);

            if (res == -1)
            {
                perror("execvp() error");
                exit(EXIT_FAILURE);
            }
        }

        exit(EXIT_SUCCESS);
    }
    else
    {

        // https://stackoverflow.com/questions/33508997/waitpid-wnohang-wuntraced-how-do-i-use-these/34845669
        if (is_background == 0)
        {
            int wstatus;
            if (waitpid(-1, &wstatus, WUNTRACED) == -1)
            {
                perror("waitpid() error");
            }
            // block parents execution and set status when process suspended/terminated/killed.

            if (WIFEXITED(wstatus))
            {
                int status = WEXITSTATUS(wstatus);

                if (status == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
            }
        }

        if (builtin_command != -1)
        {
            return builtin_commands_ptr[builtin_command](args, arg_count);
        }
    }

    return EXIT_SUCCESS;
}