#ifndef HEADERS
#define HEADERS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>
#include <time.h>
#include <sys/select.h>
#include <termios.h>
#include <fcntl.h>

#define BUFFER_SIZE 150
#define MAX_COMMANDS_STORED 20
#define MAX_HISTORY_FILE_LEN 1000
#define MAX_JOBS 500

char PATH_TO_HISTORY_FILE[BUFFER_SIZE];
char cmd_name[BUFFER_SIZE];

int foreground_pid;

struct background_jobs
{
    int pid;
    char cmd[BUFFER_SIZE];
};

static struct background_jobs ongoing_background_jobs[MAX_JOBS];

int cur_jobs;

struct simple_commands
{
    int cnt_args;
    char *cmd[BUFFER_SIZE];
    struct simple_commands *next;
};

struct simple_commands *head, *tail;

struct simple_commands *create_new_command();
void add_new_command(struct simple_commands *node);
void pop_command();
void show_commands();
void clear_commands();

int piping_and_redirection(char **args, int arg_count);

int execute_command(char **args, int);
int check_builtin_command(char *cmd);

int parse_command(char *cmd);

int prompt();
char *readline();

void child_termination_handler();
void ctrl_z_handler();
void ctrl_c_handler();

int builtin_cd(char **args, int arg_count);
int builtin_pwd(char **args, int arg_count);
int builtin_echo(char **args, int arg_count);
int builtin_ls(char **args, int arg_count);
int builtin_pinfo(char **args, int arg_count);
int builtin_history(char **args, int arg_count);
int builtin_nightswatch(char **args, int arg_count);
int quit(char **args, int arg_count);
int print_permissions(struct stat *buf);
int print_username(struct stat *buf);
int print_groupname(struct stat *buf);
int print_last_modified(struct stat *buf);
int add_to_history(char *cmd);
int interrupt();
int newborn();
int kbhit();

int add_background_jobs(char *cmd, int pid);
int remove_background_jobs(int pid);
int find_background_job(int pid);
int builtin_setenv(char **args, int arg_count);
int builtin_unsetenv(char **args, int arg_count);
int builtin_fg(char **args, int arg_count);
int builtin_bg(char **args, int arg_count);
int builtin_jobs(char **args, int arg_count);
int builtin_overkill(char **args, int arg_count);
int builtin_kjob(char **args, int arg_count);

char *trim_string(char *str);
int tokenize_commands(char *cmds, char *delimiter, char **tokenized_cmds);

static char *builtin_commands[] = {
    "cd",
    "pwd",
    "echo",
    "ls",
    "pinfo",
    "history",
    "nightswatch",
    "setenv",
    "unsetenv",
    "jobs",
    "fg",
    "bg",
    "overkill",
    "kjob",
    "quit",
};

static int (*builtin_commands_ptr[])(char **, int) = {
    builtin_cd,
    builtin_pwd,
    builtin_echo,
    builtin_ls,
    builtin_pinfo,
    builtin_history,
    builtin_nightswatch,
    builtin_setenv,
    builtin_unsetenv,
    builtin_jobs,
    builtin_fg,
    builtin_bg,
    builtin_overkill,
    builtin_kjob,
    quit,
};

#endif