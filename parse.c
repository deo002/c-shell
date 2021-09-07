#include "headers.h"
#include "parse.h"
#include "execute_command.h"
#include "builtin.h"

int parse_command(char *cmd)
{
    if (add_to_history(cmd) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    char *token = strtok(cmd, " \t\n");

    char **arguments = (char **)malloc(MAX_ARG_NUM * sizeof(char *));

    for (int i = 0; i < MAX_ARG_NUM; i++)
    {
        arguments[i] = (char *)malloc(MAX_ARG_LEN * sizeof(char));

        if (arguments[i] == NULL)
        {
            fprintf(stderr, "Error allocating memory");

            for (int j = 0; j < i; j++)
            {
                free(arguments[j]);
            }

            free(arguments);

            return EXIT_FAILURE;
        }
    }

    int arg_count = 0;

    if (strcmp(token, "exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(token, "echo") == 0)
    {
        strcpy(arguments[arg_count++], token);
        token = strtok(NULL, "\0");
        if (token != NULL)
        {
            strcpy(arguments[arg_count++], token);
        }
    }
    else
    {
        while (token != NULL)
        {
            strcpy(arguments[arg_count++], token);

            if (arg_count + 1 >= MAX_ARG_NUM)
            {
                fprintf(stderr, "Argument limit exceeded\n");

                for (int i = 0; i < arg_count; i++)
                {
                    free(arguments[i]);
                }

                free(arguments);

                return EXIT_FAILURE;
            }

            token = strtok(NULL, " \t\n");
        }
    }

    int res = execute_command(arguments, arg_count);

    for (int i = 0; i < arg_count; i++)
    {
        free(arguments[i]);
    }

    free(arguments);

    return res;
}
