#ifndef BUILTIN_H
#define BUILTIN_H

#define MAX_COMMANDS_STORED 20
#define MAX_HISTORY_FILE_LEN 1000

#include <sys/stat.h>

int builtin_cd(char **args, int arg_count);
int builtin_pwd(char **args, int arg_count);
int builtin_echo(char **args, int arg_count);
int builtin_ls(char **args, int arg_count);
int builtin_pinfo(char **args, int arg_count);
int builtin_history(char **args, int arg_count);
int builtin_nightswatch(char **args, int arg_count);
int print_permissions(struct stat *buf);
int print_username(struct stat *buf);
int print_groupname(struct stat *buf);
int print_last_modified(struct stat *buf);
int add_to_history(char *cmd);
int interrupt();
int newborn();
int kbhit();

static const char *builtin_commands[] = {
    "cd",
    "pwd",
    "echo",
    "ls",
    "pinfo",
    "history",
    "nightswatch",
};

static const int (*builtin_commands_ptr[])(char **, int) = {
    builtin_cd,
    builtin_pwd,
    builtin_echo,
    builtin_ls,
    builtin_pinfo,
    builtin_history,
    builtin_nightswatch,
};

#endif // BUILTIN