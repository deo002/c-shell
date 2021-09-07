#include "headers.h"
#include "commands.h"

commands_t *generate_list()
{
    commands_t *list = (commands_t *)malloc(sizeof(commands_t));
    list->head = NULL;
    list->tail = NULL;

    return list;
}

commands_rec_t *generate_command(char *cmd)
{
    commands_rec_t *node = (commands_rec_t *)malloc(sizeof(commands_rec_t));
    strcpy(node->command, cmd);
    node->next = NULL;

    return node;
}

void print_commands(commands_t *comm)
{
    if (comm == NULL)
        return;

    commands_rec_t *ptr = comm->head;

    while (ptr != NULL)
    {
        printf("%s\n", ptr->command);
        ptr = ptr->next;
    }
}

void insert_command(commands_t *comm, char *cmd)
{
    if (comm == NULL)
        return;

    if (!cmd)
        return;

    commands_rec_t *node = generate_command(cmd);

    if (comm->head == NULL)
    {
        comm->head = node;
        comm->tail = node;
    }
    else
    {
        (comm->tail)->next = node;
        comm->tail = node;
    }
}

void delete_commands(commands_t *comm)
{
    commands_rec_t *ptr = comm->head;

    while (ptr != NULL)
    {
        commands_rec_t *temp = ptr;
        ptr = ptr->next;
        temp->next = NULL;
        free(temp);
    }

    free(comm);
}