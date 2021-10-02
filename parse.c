#include "headers.h"

int parse_command(char *cmd)
{
    if (add_to_history(cmd) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    char **arguments = (char **)malloc(BUFFER_SIZE * sizeof(char *));

    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        arguments[i] = (char *)malloc(BUFFER_SIZE);

        if (arguments[i] == NULL)
        {
            fprintf(stderr, "Error allocating memory\n");

            for (int j = 0; j < i; ++j)
            {
                free(arguments[j]);
            }

            free(arguments);
            return EXIT_FAILURE;
        }
    }

    char delimiter[5];
    delimiter[0] = ' ';
    delimiter[1] = '\t';
    delimiter[2] = '\n';
    delimiter[3] = '\0';

    int arg_count = tokenize_commands(cmd, delimiter, arguments);

    if (arg_count == -1)
    {
        for (int i = 0; i < BUFFER_SIZE; ++i)
        {
            free(arguments[i]);
        }

        free(arguments);

        return EXIT_FAILURE;
    }

    int res = execute_command(arguments, arg_count);

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        free(arguments[i]);
    }

    free(arguments);

    return res;
}
