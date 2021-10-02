#include "headers.h"

int builtin_cd(char **args, int arg_count)
{
    if (arg_count == 1)
    {
        strcpy(args[arg_count++], "~");
    }
    else if (arg_count > 2)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    if (strcmp(args[1], "~") == 0)
    {
        if (chdir(getenv("HOME")) != 0)
        {
            perror("chdir() error");
            return EXIT_FAILURE;
        }
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("chdir() failed");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int builtin_pwd(char **args, int arg_count)
{
    if (arg_count != 1)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    size_t alloc_sz = BUFFER_SIZE * sizeof(char);

    char *cwd = (char *)malloc(alloc_sz);

    if (cwd == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        return EXIT_FAILURE;
    }

    if (getcwd(cwd, alloc_sz) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("getcwd() error");

        free(cwd);

        return EXIT_FAILURE;
    }

    free(cwd);

    return EXIT_SUCCESS;
}

int builtin_echo(char **args, int arg_count)
{
    if (arg_count == 1)
    {
        printf("\n");
    }
    else
    {
        for (int i = 1; i < arg_count; ++i)
        {
            printf("%s ", args[i]);
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}

int builtin_pinfo(char **args, int arg_count)
{
    if (arg_count > 2)
    {
        fprintf(stderr, "Too many arguments");
        return EXIT_FAILURE;
    }

    int pid;

    if (arg_count == 2)
    {
        long num = 0;
        char *end = NULL;

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

        pid = (int)num;
    }
    else
    {
        pid = getpid();
    }

    char *path = (char *)malloc(BUFFER_SIZE);

    if (path == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        return EXIT_FAILURE;
    }

    sprintf(path, "/proc/%d/stat", pid);

    FILE *fp;

    fp = fopen(path, "r");

    if (fp == NULL)
    {
        perror("fopen() error");
        return EXIT_FAILURE;
    }

    free(path);

    char *exe = (char *)malloc(BUFFER_SIZE);

    if (exe == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        return EXIT_FAILURE;
    }

    char state;
    long unsigned int v_size;

    fscanf(fp, "%*d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*u %lu",
           exe, &state, &v_size);

    fclose(fp);

    printf("pid             ---> %d\n", pid);
    printf("Process Status  ---> %c\n", state);
    printf("Memory          ---> %lu\n", v_size);
    printf("Executable path ---> %s\n", exe);

    free(exe);

    return EXIT_SUCCESS;
}

int quit(char **args, int arg_count)
{
    exit(EXIT_SUCCESS);
}