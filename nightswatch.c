#include "headers.h"

int builtin_nightswatch(char **args, int arg_count)
{
    if (arg_count > 4)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    if (arg_count < 2)
    {
        fprintf(stderr, "Missing arguments\n");
        return EXIT_FAILURE;
    }

    int interval = 5;
    int flag = -1; // 0 for interrupt and 1 for newborn

    if (strcmp(args[1], "-n") == 0)
    {
        if (arg_count == 3)
        {
            fprintf(stderr, "Missing arguments\n");
            return EXIT_FAILURE;
        }

        long num = 0;
        char *end = NULL;

        num = strtol(args[2], &end, 10);

        if (end == args[2])
        {
            fprintf(stderr, "Not an integer argument\n");
            return EXIT_FAILURE;
        }

        if ((num > INT_MAX) || (num < 0))
        {
            fprintf(stderr, "Number out of range\n");
            return EXIT_FAILURE;
        }

        interval = (int)num;

        if (strcmp(args[3], "interrupt") == 0)
        {
            flag = 0;
        }
        else if (strcmp(args[3], "newborn") == 0)
        {
            flag = 1;
        }
    }
    else
    {
        if (strcmp(args[1], "interrupt") == 0)
        {
            flag = 0;
        }
        else if (strcmp(args[1], "newborn") == 0)
        {
            flag = 1;
        }
    }

    if (flag == -1)
    {
        fprintf(stderr, "Agruments interrupt or newborn expected\n");
        return EXIT_FAILURE;
    }

    time_t begin_time = time(NULL);

    double count = 0.0;

    while (1)
    {
        time_t cur_time = time(NULL);

        if (difftime(cur_time, begin_time) / (double)interval >= count)
        {
            if (flag == 0)
            {
                if (interrupt() == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
            }
            else
            {
                if (newborn() == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
            }
            count += 1.0;
        }

        struct termios orig_termios, new_termios;

        tcgetattr(0, &orig_termios);
        memcpy(&new_termios, &orig_termios, sizeof(new_termios));

        cfmakeraw(&new_termios);
        tcsetattr(0, TCSANOW, &new_termios);

        if (kbhit() && getchar() == 'q')
        {
            tcsetattr(0, TCSANOW, &orig_termios);
            break;
        }

        tcsetattr(0, TCSANOW, &orig_termios);
    }

    return EXIT_SUCCESS;
}

int newborn()
{
    FILE *fp = fopen("/proc/loadavg", "r");

    if (fp == NULL)
    {
        perror("fopen() error");
        return EXIT_FAILURE;
    }

    char *line = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    if (line == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        fclose(fp);
        return EXIT_FAILURE;
    }

    fgets(line, BUFFER_SIZE, fp);

    int ind = strlen(line) - 1;

    while (ind > 0 && line[ind] != ' ')
    {
        --ind;
    }
    fflush(stdout);
    printf("%s\n", line + ind + 1);

    fclose(fp);
    free(line);

    return EXIT_SUCCESS;
}

int interrupt()
{
    FILE *fp = fopen("/proc/interrupts", "r");

    if (fp == NULL)
    {
        perror("fopen() error");
        return EXIT_FAILURE;
    }

    char *line = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    if (line == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        fclose(fp);
        return EXIT_FAILURE;
    }

    rewind(fp);
    int line_num = 0;

    while (fgets(line, BUFFER_SIZE, fp))
    {
        if (line_num == 0)
        {
            fflush(stdout);
            printf("%s", line + 3);
        }
        if (line_num == 2)
        {
            fflush(stdout);
            printf("%s\n", line + 6);
            break;
        }
        ++line_num;
    }

    fclose(fp);
    free(line);

    return EXIT_SUCCESS;
}

int kbhit()
{
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}
