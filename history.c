#include "headers.h"

int builtin_history(char **args, int arg_count)
{
    if (arg_count > 2)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(PATH_TO_HISTORY_FILE, "r");

    if (fp == NULL)
    {
        perror("fopen() error");
        return EXIT_FAILURE;
    }

    char *content = (char *)malloc(sizeof(char) * MAX_HISTORY_FILE_LEN);

    if (content == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        fclose(fp);
        return EXIT_FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(fp);
    rewind(fp);

    fread(content, len, 1, fp);
    content[len] = '\0';

    char *token = strtok(content, "\n");

    long num = 0;
    char *end = NULL;

    num = strtol(token, &end, 10);

    if (end == token)
    {
        fprintf(stderr, "Not an integer argument\n");
        return EXIT_FAILURE;
    }

    if ((num > INT_MAX) || (num < 0))
    {
        fprintf(stderr, "Number out of range\n");
        return EXIT_FAILURE;
    }

    int cnt = (int)num;

    int iter = cnt;

    if (arg_count == 2)
    {
        int temp = 0;

        num = strtol(args[1], &end, 10);

        if (end == args[1])
        {
            fprintf(stderr, "Not an integer argument\n");
            return EXIT_FAILURE;
        }

        if ((num > INT_MAX) || (num < 0))
        {
            fprintf(stderr, "Number out of range\n");
            return EXIT_FAILURE;
        }

        temp = (int)num;
        if (cnt > temp)
        {
            cnt = temp;
        }
    }

    token = strtok(NULL, "\n");

    while (token != NULL && iter != 0)
    {
        if (iter <= cnt)
        {
            printf("%s\n", token);
        }
        token = strtok(NULL, "\n");
        --iter;
    }

    free(content);
    fclose(fp);

    return EXIT_SUCCESS;
}

int add_to_history(char *cmd)
{
    FILE *fp = fopen(PATH_TO_HISTORY_FILE, "r");

    if (fp == NULL)
    {
        perror("fopen() error");
        return EXIT_FAILURE;
    }

    char *content = (char *)malloc(sizeof(char) * MAX_HISTORY_FILE_LEN);

    if (content == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        fclose(fp);
        return EXIT_FAILURE;
    }

    char *num = (char *)malloc(sizeof(char) * 5);

    if (num == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        fclose(fp);
        return EXIT_FAILURE;
    }

    char *first_cmd = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    if (first_cmd == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        fclose(fp);
        return EXIT_FAILURE;
    }

    char *rest_cmds = (char *)malloc(sizeof(char) * MAX_HISTORY_FILE_LEN);

    if (rest_cmds == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        fclose(fp);
        return EXIT_FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(fp);
    rewind(fp);

    if (len > 0)
    {
        fread(content, len, 1, fp);
        content[len] = '\0';
    }
    else
    {
        strcpy(content, "0\n");
    }

    char *token = strtok(content, "\n");
    strcpy(num, token);

    token = strtok(NULL, "\n");
    if (token != NULL)
    {
        strcpy(first_cmd, token);
        token = strtok(NULL, "\0");

        if (token != NULL)
        {
            strcpy(rest_cmds, token);
            token = strtok(NULL, "\0");
        }
    }

    fp = freopen(NULL, "w", fp);

    long n = 0;
    char *end = NULL;

    n = strtol(num, &end, 10);

    if (end == num)
    {
        fprintf(stderr, "Not an integer argument\n");
        return EXIT_FAILURE;
    }

    if ((n > INT_MAX) || (n < 0))
    {
        fprintf(stderr, "Number out of range\n");
        return EXIT_FAILURE;
    }

    int no = (int)n;

    if (no == MAX_COMMANDS_STORED)
    {
        fprintf(fp, "%s\n%s\n%s", num, rest_cmds, cmd);
    }
    else
    {
        no++;
        if (no == 1)
        {
            fprintf(fp, "%d\n%s", no, cmd);
        }
        else if (no == 2)
        {
            fprintf(fp, "%d\n%s\n%s", no, first_cmd, cmd);
        }
        else
        {
            printf("%s\n", cmd);
            fprintf(fp, "%d\n%s\n%s\n%s", no, first_cmd, rest_cmds, cmd);
        }
    }

    free(num);
    free(first_cmd);
    free(rest_cmds);
    fclose(fp);

    return EXIT_SUCCESS;
}
