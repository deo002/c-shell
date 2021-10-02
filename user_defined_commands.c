#include "headers.h"

int builtin_setenv(char **args, int arg_count)
{
    if (arg_count > 3)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    if (arg_count == 1)
    {
        fprintf(stderr, "Arguments missing\n");
        return EXIT_FAILURE;
    }

    if (arg_count == 2)
    {
        args[arg_count++][0] = '\0';
    }

    int rc = setenv(args[1], args[2], 1);

    if (rc == -1)
    {
        perror("setenv() error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int builtin_unsetenv(char **args, int arg_count)
{
    if (arg_count > 2)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    if (arg_count == 1)
    {
        fprintf(stderr, "Arguments missing\n");
        return EXIT_FAILURE;
    }

    int rc = unsetenv(args[1]);

    if (rc == -1)
    {
        perror("unsetenv() error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int add_background_jobs(char *cmd, int pid)
{
    if (cur_jobs >= MAX_JOBS)
    {
        fprintf(stderr, "Background jobs limit reached\n");
        return EXIT_FAILURE;
    }

    strcpy(ongoing_background_jobs[cur_jobs].cmd, cmd);
    ongoing_background_jobs[cur_jobs].pid = pid;
    ++cur_jobs;

    return EXIT_SUCCESS;
}

int remove_background_jobs(int pid)
{
    if (cur_jobs == 0)
    {
        fprintf(stderr, "No background jobs\n");
        return EXIT_FAILURE;
    }

    int found = -1;

    for (int i = 0; i < cur_jobs; ++i)
    {
        if (pid == ongoing_background_jobs[i].pid)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        fprintf(stderr, "Job with pid [%d] not found\n", pid);
        return EXIT_FAILURE;
    }

    for (int i = found + 1; i < cur_jobs; ++i)
    {
        ongoing_background_jobs[i - 1] = ongoing_background_jobs[i];
    }

    --cur_jobs;

    return EXIT_SUCCESS;
}

int find_background_job(int pid)
{
    int found = -1;

    for (int i = 0; i < cur_jobs; ++i)
    {
        if (ongoing_background_jobs[i].pid == pid)
        {
            found = i;
            break;
        }
    }

    return found;
}

int builtin_jobs(char **args, int arg_count)
{
    if (arg_count > 1)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    char *status = (char *)malloc(BUFFER_SIZE);

    if (status == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < cur_jobs; ++i)
    {
        char *path_to_file = (char *)malloc(BUFFER_SIZE);

        if (path_to_file == NULL)
        {
            fprintf(stderr, "Error allocating memory\n");
            free(status);
            return EXIT_FAILURE;
        }

        sprintf(path_to_file, "/proc/%d/stat", ongoing_background_jobs[i].pid);

        FILE *fp = NULL;

        fp = fopen(path_to_file, "r");

        if (fp == NULL)
        {
            perror("fopen() error");
            free(status);
            return EXIT_FAILURE;
        }

        free(path_to_file);

        char state;

        fscanf(fp, "%*d %*s %c", &state);

        fclose(fp);

        if (state == 'S')
        {
            strcpy(status, "Running");
        }
        else
        {
            strcpy(status, "Stopped");
        }

        printf("[%d] %s %s [%d]\n", i + 1, status, ongoing_background_jobs[i].cmd, ongoing_background_jobs[i].pid);
    }

    free(status);

    return EXIT_SUCCESS;
}

int builtin_fg(char **args, int arg_count)
{
    if (arg_count > 2)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    int job_number;

    if (arg_count == 1)
    {
        job_number = cur_jobs - 1;
    }
    else
    {
        long num = 0;
        char *end = NULL;

        num = strtol(args[1], &end, 10);

        if (end == args[1])
        {
            fprintf(stderr, "Not an integer argument\n");
            return EXIT_FAILURE;
        }

        if ((num > INT_MAX) || (num <= 0))
        {
            fprintf(stderr, "Number out of range\n");
            return EXIT_FAILURE;
        }

        job_number = (int)num;
        --job_number;
    }

    if (job_number >= cur_jobs)
    {
        fprintf(stderr, "No such job exists\n");
        return EXIT_FAILURE;
    }

    int pid = ongoing_background_jobs[job_number].pid;

    foreground_pid = pid;
    strcpy(cmd_name, ongoing_background_jobs[job_number].cmd);

    int rc = remove_background_jobs(pid);

    if (rc == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    rc = kill(pid, SIGCONT);

    if (rc == -1)
    {
        perror("kill() error");
        return EXIT_FAILURE;
    }

    printf("brings [%d] to the foreground\n", job_number + 1);

    int wstatus;
    if (waitpid(-1, &wstatus, WUNTRACED) == -1)
    {
        perror("waitpid() error");
    }

    if (WIFEXITED(wstatus))
    {
        int status = WEXITSTATUS(wstatus);

        if (status == EXIT_FAILURE)
        {
            foreground_pid = -1;
            strcpy(cmd_name, "");
            return EXIT_FAILURE;
        }
    }

    foreground_pid = -1;
    strcpy(cmd_name, "");
    return EXIT_SUCCESS;
}

int builtin_bg(char **args, int arg_count)
{
    if (arg_count > 2)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    int job_number;

    if (arg_count == 1)
    {
        job_number = cur_jobs - 1;
    }
    else
    {
        long num = 0;
        char *end = NULL;

        num = strtol(args[1], &end, 10);

        if (end == args[1])
        {
            fprintf(stderr, "Not an integer argument\n");
            return EXIT_FAILURE;
        }

        if ((num > INT_MAX) || (num <= 0))
        {
            fprintf(stderr, "Number out of range\n");
            return EXIT_FAILURE;
        }

        job_number = (int)num;
        --job_number;
    }

    if (job_number >= cur_jobs)
    {
        fprintf(stderr, "No such job exists\n");
        return EXIT_FAILURE;
    }

    int pid = ongoing_background_jobs[job_number].pid;

    int rc = kill(pid, SIGCONT);

    if (rc == -1)
    {
        perror("kill() error");
        return EXIT_FAILURE;
    }

    printf("changes state of [%d] to running in background\n", job_number + 1);

    return EXIT_SUCCESS;
}

int builtin_overkill(char **args, int arg_count)
{
    if (arg_count > 1)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < cur_jobs; ++i)
    {
        int rc = kill(ongoing_background_jobs[i].pid, SIGKILL);

        if (rc == -1)
        {
            perror("kill() error");
            return EXIT_FAILURE;
        }
    }

    cur_jobs = 0;

    return EXIT_SUCCESS;
}

int builtin_kjob(char **args, int arg_count)
{
    if (arg_count < 3)
    {
        fprintf(stderr, "Arguments missing\n");
        return EXIT_FAILURE;
    }

    if (arg_count > 3)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    int job_num, sig_num;

    long num = 0;
    char *end = NULL;

    num = strtol(args[1], &end, 10);

    if (end == args[1])
    {
        fprintf(stderr, "Not an integer argument\n");
        return EXIT_FAILURE;
    }

    if ((num > INT_MAX) || (num <= 0))
    {
        fprintf(stderr, "Number out of range\n");
        return EXIT_FAILURE;
    }

    job_num = (int)num;
    --job_num;

    end = NULL;

    num = strtol(args[2], &end, 10);

    if (end == args[2])
    {
        fprintf(stderr, "Not an integer argument\n");
        return EXIT_FAILURE;
    }

    if ((num > INT_MAX) || (num <= 0))
    {
        fprintf(stderr, "Number out of range\n");
        return EXIT_FAILURE;
    }

    sig_num = (int)num;

    if (job_num >= cur_jobs)
    {
        fprintf(stderr, "No such job exists\n");
        return EXIT_FAILURE;
    }

    int rc = kill(ongoing_background_jobs[job_num].pid, sig_num);

    if (rc == -1)
    {
        perror("kill() error");
        return EXIT_FAILURE;
    }
    else
    {
        printf("Signal %d was sent to job %d\n", sig_num, job_num + 1);
    }

    if (sig_num == SIGKILL || sig_num == SIGTERM || sig_num == SIGINT)
    {
        rc = remove_background_jobs(ongoing_background_jobs[job_num].pid);

        if (rc == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}