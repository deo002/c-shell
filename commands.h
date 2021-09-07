#ifndef COMMAND_H
#define COMMAND_H

#define COMMAND_MAX_SIZE 128

typedef struct commands_rec
{
    char command[COMMAND_MAX_SIZE];
    struct commands_rec *next;
} commands_rec_t;

typedef struct commands
{
    commands_rec_t *head;
    commands_rec_t *tail;
} commands_t;

commands_t *generate_list();

commands_rec_t *generate_command(char *cmd);

void print_commands(commands_t *comm);

void insert_command(commands_t *comm, char *cmd);

void delete_commands(commands_t *comm);

#endif // COMMAND