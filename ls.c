#include "headers.h"

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