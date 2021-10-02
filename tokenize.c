#include "headers.h"

char *trim_string(char *str)
{
    char *end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

    return str;
}

int tokenize_commands(char *cmds, char *delimiter, char **tokenized_cmds)
{
    char *token = strtok(cmds, delimiter);

    int count = 0;

    while (token != NULL)
    {
        if (count >= BUFFER_SIZE)
        {
            fprintf(stderr, "Command limit exceeded\n");
            return -1;
        }

        token = trim_string(token);

        strcpy(tokenized_cmds[count], token);
        token = strtok(NULL, delimiter);
        ++count;
    }

    return count;
}