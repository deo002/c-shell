#include "headers.h"

int prompt()
{
    char *login_name = (char *)malloc(BUFFER_SIZE);

    if (login_name == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        return EXIT_FAILURE;
    }

    char *host_name = (char *)malloc(BUFFER_SIZE);

    if (host_name == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        free(login_name);
        return EXIT_FAILURE;
    }

    if (getlogin() == NULL)
    {
        login_name = "anonymous_user";
    }
    else
    {
        strcpy(login_name, getlogin());
    }

    int result = gethostname(host_name, BUFFER_SIZE);

    if (result == -1)
    {
        perror("Error getting hostname");
    }

    char *path = (char *)malloc(BUFFER_SIZE);

    if (path == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        return EXIT_FAILURE;
    }

    if (getcwd(path, BUFFER_SIZE) != NULL)
    {
        char *homedir = getenv("HOME");
        int len = strlen(homedir);

        for (int i = len; i <= strlen(path); ++i)
        {
            path[i - len] = path[i];
        }
    }
    else
    {
        perror("getcwd() error");

        free(path);
        free(login_name);
        free(host_name);

        return EXIT_FAILURE;
    }

    printf("\033[0;35m<%s@%s:\033[0;32m%s\033[0;35m~>\033[0;37m", host_name, login_name, path);

    free(login_name);
    free(host_name);
    free(path);

    return EXIT_SUCCESS;
}

char *readline()
{
    char *command = (char *)malloc(BUFFER_SIZE);

    if (command == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        return command;
    }

    size_t sz = BUFFER_SIZE;

    if (getline(&command, &sz, stdin) == -1)
    {
        perror("getline()");
    }

    if (strlen(command) > 0)
    {
        command[strlen(command) - 1] = '\0';
    }

    return command;
}