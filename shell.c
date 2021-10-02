#include "headers.h"

int main(int argc, char **argv)
{
    foreground_pid = -1;
    cur_jobs = 0;
    strcpy(cmd_name, "");

    if (getcwd(PATH_TO_HISTORY_FILE, BUFFER_SIZE) == NULL)
    {
        perror("getcwd() error");
        return EXIT_FAILURE;
    }

    strcat(PATH_TO_HISTORY_FILE, "/history.txt");

    signal(SIGCHLD, child_termination_handler);
    signal(SIGTSTP, ctrl_z_handler);
    signal(SIGINT, ctrl_c_handler);

    while (1)
    {
        if (prompt() == EXIT_FAILURE)
        {
            fprintf(stderr, "Error starting shell\n");
            return EXIT_FAILURE;
        }

        char *command = readline();

        if (command == NULL)
        {
            continue;
        }

        char **tokenized_commands = (char **)malloc(sizeof(char *) * BUFFER_SIZE);

        if (tokenized_commands == NULL)
        {
            fprintf(stderr, "Error allocating memory");
            free(command);
            continue;
        }

        int memory_allocation_error = 0;

        for (int i = 0; i < BUFFER_SIZE; ++i)
        {
            tokenized_commands[i] = (char *)malloc(BUFFER_SIZE);

            if (tokenized_commands[i] == NULL)
            {
                memory_allocation_error = 1;
                fprintf(stderr, "Error allocating memory\n");

                for (int j = 0; j < i; ++j)
                {
                    free(tokenized_commands[j]);
                }

                free(tokenized_commands);
                free(command);
                fflush(stdout);

                break;
            }
        }

        if (memory_allocation_error == 1)
        {
            continue;
        }

        char delimiter[2];
        delimiter[0] = ';';
        delimiter[1] = '\0';

        int comm_count = tokenize_commands(command, delimiter, tokenized_commands);

        if (comm_count != -1)
        {
            for (int i = 0; i < comm_count; ++i)
            {
                int rc = parse_command(tokenized_commands[i]);
            }
        }

        for (int i = 0; i < BUFFER_SIZE; ++i)
        {
            free(tokenized_commands[i]);
        }

        free(command);
        free(tokenized_commands);
        fflush(stdout);
    }

    return 0;
}