#include "headers.h"

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

int check_piping_and_redirection(char **args, int arg_count)
{
    for (int i = 0; i < arg_count; ++i)
    {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], "<") == 0 || strcmp(args[i], "|") == 0)
            return 1;
    }

    return 0;
}

int execute_command(char **args, int arg_count)
{
    if (check_piping_and_redirection(args, arg_count) == 1)
    {
        return piping_and_redirection(args, arg_count);
    }

    strcpy(cmd_name, args[0]);

    int builtin_command = check_builtin_command(args[0]);

    int is_background = 0;

    if (strcmp(args[arg_count - 1], "&") == 0)
    {
        is_background = 1;
        --arg_count;
    }

    if (builtin_command != -1)
    {
        foreground_pid = -1;
        strcpy(cmd_name, "");
        return builtin_commands_ptr[builtin_command](args, arg_count);
    }

    free(args[arg_count]);
    args[arg_count] = NULL;

    int pid = fork();

    if (pid == -1)
    {
        perror("fork() error");
        return EXIT_FAILURE;
    }

    if (pid == 0)
    {
        setpgid(0, 0);
        int res = execvp(args[0], args);

        if (res == -1)
        {
            perror("execvp() error");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        if (is_background == 0)
        {
            foreground_pid = pid;
            int wstatus;
            if (waitpid(-1, &wstatus, WUNTRACED) == -1)
            {
                perror("waitpid() error");
            }

            if (WIFEXITED(wstatus))
            {
                int status = WEXITSTATUS(wstatus);
                if (status == EXIT_FAILURE)
                {
                    foreground_pid = -1;
                    return EXIT_FAILURE;
                }
            }
        }
        else
        {
            int rc = add_background_jobs(args[0], pid);

            if (rc == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }
        }
    }

    foreground_pid = -1;
    strcpy(cmd_name, "");
    return EXIT_SUCCESS;
}
