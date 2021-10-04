#include "headers.h"

struct simple_commands *create_new_command()
{
    struct simple_commands *node = (struct simple_commands *)malloc(sizeof(struct simple_commands));

    return node;
}

void add_new_command(struct simple_commands *node)
{
    if (head == NULL && tail == NULL)
    {
        head = node;
        tail = node;
    }
    else
    {
        tail->next = node;
        tail = node;
    }
}

void pop_command()
{
    if (head == tail && head == NULL)
    {
        return;
    }
    else if (head == tail && head != NULL)
    {
        struct simple_commands *temp = head;
        head = NULL;
        tail = NULL;
        free(temp);
    }
    else
    {
        struct simple_commands *temp = head;
        head = head->next;
        temp->next = NULL;
        free(temp);
    }
}

void show_commands()
{
    struct simple_commands *p = head;

    while (p != NULL)
    {
        int cnt = p->cnt_args;
        for (int i = 0; i < cnt; ++i)
        {
            printf("%s ", p->cmd[i]);
        }
        printf("\n");
        p = p->next;
    }
}

void clear_commands()
{
    while (head != NULL)
    {
        for (int i = 0; i < head->cnt_args; ++i)
        {
            free(head->cmd[i]);
        }

        pop_command();
    }
}