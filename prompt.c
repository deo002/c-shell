#include "headers.h"
#include "prompt.h"
#include "tokenize.h"
#include "commands.h"
#include "parse.h"

int prompt()
{
    size_t alloc_sz = BUFFER_SIZE * sizeof(char);

    char *login_name = (char *)malloc(alloc_sz);

    if (login_name == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        return EXIT_FAILURE;
    }

    char *host_name = (char *)malloc(alloc_sz);

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

    char *path = (char *)malloc(alloc_sz);

    if (path == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        return EXIT_FAILURE;
    }

    if (getcwd(path, alloc_sz) != NULL)
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

void readline()
{
    char *command = (char *)malloc(BUFFER_SIZE * sizeof(char));

    if (command == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        return;
    }

    size_t sz = BUFFER_SIZE * sizeof(char);

    if (getline(&command, &sz, stdin) == -1)
    {
        perror("getline()");
    }

    if (strlen(command) > 0)
    {
        command[strlen(command) - 1] = '\0';
    }

    fflush(stdout);

    commands_t *list = tokenize_commands(command);

    commands_rec_t *ptr = list->head;

    while (ptr != NULL)
    {
        int rc = parse_command(ptr->command);
        ptr = ptr->next;
    }

    delete_commands(list);
    free(command);
}