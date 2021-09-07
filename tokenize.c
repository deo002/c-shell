#include "headers.h"
#include "tokenize.h"

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

commands_t *tokenize_commands(char *cmds)
{
    commands_t *list = generate_list();

    char *s;
    strcpy(s, cmds);

    char *token = strtok(s, ";");

    while (token != NULL)
    {
        token = trim_string(token);
        insert_command(list, token);
        token = strtok(NULL, ";");
    }

    return list;
}