#include "headers.h"
#include "builtin.h"

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
    if (arg_count > 2)
    {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    }

    if (arg_count == 1)
    {
        printf("\n");
    }
    else
    {
        printf("%s\n", args[1]);
    }

    return EXIT_SUCCESS;
}

int builtin_ls(char **args, int arg_count)
{
    DIR *dir_ptr = NULL;
    struct dirent *sd = NULL;
    struct stat statbuf;

    int long_format = 0, show_hidden_files = 0;

    char *dir = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    if (dir == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        return EXIT_FAILURE;
    }

    strcpy(dir, ".");

    for (int i = 1; i < arg_count; ++i)
    {
        if (strcmp("-l", args[i]) == 0)
            long_format = 1;
        else if (strcmp("-a", args[i]) == 0)
            show_hidden_files = 1;
        else if (strcmp("-al", args[i]) == 0)
        {
            long_format = 1;
            show_hidden_files = 1;
        }
        else if (strcmp("..", args[i]) == 0)
        {
            strcpy(dir, "..");
        }
        else if (strcmp(".", args[i]) == 0)
            continue;
        else if (strcmp("~", args[i]) == 0)
        {
            strcpy(dir, getenv("HOME"));
        }
        else
        {
            if (strcmp(dir, ".") == 0)
            {
                strcat(dir, "/");
                strcat(dir, args[1]);
            }
        }
    }

    dir_ptr = opendir(dir);
    free(dir);

    if (dir_ptr == NULL)
    {
        perror("opendir() error");
        return EXIT_FAILURE;
    }

    while ((sd = readdir(dir_ptr)) != NULL)
    {
        if (show_hidden_files == 0 && sd->d_name[0] == '.')
        {
            continue;
        }

        if (long_format == 1)
        {
            if (stat(sd->d_name, &statbuf) == -1)
            {
                perror("stat() error");
                return EXIT_FAILURE;
            }

            if (print_permissions(&statbuf) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            printf("\033[0;37m%lu ", statbuf.st_nlink);

            if (print_groupname(&statbuf) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            if (print_username(&statbuf) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            printf("\033[0;37m%lu ", statbuf.st_size);

            if (print_last_modified(&statbuf) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }
        }

        if (sd->d_type == DT_DIR)
        {
            printf("\033[0;34m%s\n", sd->d_name);
        }
        else
        {
            printf("\033[0;37m%s\n", sd->d_name);
        }
    }

    closedir(dir_ptr);

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

    char *path = (char *)malloc(sizeof(char) * BUFFER_SIZE);

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

    char *exe = (char *)malloc(sizeof(char) * BUFFER_SIZE);

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

int print_permissions(struct stat *buf)
{
    struct stat statbuf = *buf;
    char *permissions = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    if (permissions == NULL)
    {
        fprintf(stderr, "Error printing memory");
        return EXIT_FAILURE;
    }

    strcpy(permissions, "----------");

    if (S_ISDIR(statbuf.st_mode))
    {
        permissions[0] = 'd';
    }

    if (statbuf.st_mode & S_IRUSR)
    {
        permissions[1] = 'r';
    }
    if (statbuf.st_mode & S_IWUSR)
    {
        permissions[2] = 'w';
    }
    if (statbuf.st_mode & S_IXUSR)
    {
        permissions[3] = 'x';
    }

    if (statbuf.st_mode & S_IRGRP)
    {
        permissions[4] = 'r';
    }
    if (statbuf.st_mode & S_IWGRP)
    {
        permissions[5] = 'w';
    }
    if (statbuf.st_mode & S_IXGRP)
    {
        permissions[6] = 'x';
    }

    if (statbuf.st_mode & S_IROTH)
    {
        permissions[7] = 'r';
    }
    if (statbuf.st_mode & S_IWOTH)
    {
        permissions[8] = 'w';
    }
    if (statbuf.st_mode & S_IXOTH)
    {
        permissions[9] = 'x';
    }

    printf("\033[0;37m%s ", permissions);
    free(permissions);

    return EXIT_SUCCESS;
}

int print_groupname(struct stat *buf)
{
    struct stat statbuf = *buf;
    struct group *grp;
    grp = getgrgid(statbuf.st_gid);
    if (grp == NULL)
    {
        perror("getgrgid() error");
        return EXIT_FAILURE;
    }
    printf("\033[0;37m%s ", grp->gr_name);

    return EXIT_SUCCESS;
}

int print_username(struct stat *buf)
{
    struct stat statbuf = *buf;

    struct passwd *pws;
    pws = getpwuid(statbuf.st_uid);
    if (pws == NULL)
    {
        perror("getpwuid() error");
        return EXIT_FAILURE;
    }
    printf("\033[0;37m%s ", pws->pw_name);

    return EXIT_SUCCESS;
}

int print_last_modified(struct stat *buf)
{
    struct stat statbuf = *buf;

    char *mtime = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    if (mtime == NULL)
    {
        fprintf(stderr, "Error printing memory");
        return EXIT_FAILURE;
    }

    strcpy(mtime, ctime(&statbuf.st_mtime));
    mtime[strlen(mtime) - 1] = '\0';
    printf("\033[0;37m%s ", mtime);
    free(mtime);

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
