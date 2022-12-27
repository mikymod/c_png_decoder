#include <stddef.h> // required for NULL
#include <stdlib.h>

#include "linked_list.h"

struct list_node *list_get_tail(struct list_node **head)
{
    struct list_node *current_node = *head;
    struct list_node *last_node = NULL;
    while (current_node)
    {
        last_node = current_node;
        current_node = current_node->next;
    }
    return last_node;
}

struct list_node *list_append(struct list_node **head, struct list_node *item)
{
    struct list_node *tail = list_get_tail(head);
    if (!tail)
    {
        *head = item;
    }
    else
    {
        tail->next = item;
    }
    item->next = NULL;
    return item;
}

struct list_node *list_pop(struct list_node **head)
{
    struct list_node *current_head = *head;
    if (!current_head)
    {
        return NULL;
    }
    *head = (*head)->next;
    current_head->next = NULL;
    return current_head;
}

struct list_node *list_get_node(struct list_node **head, const unsigned int index)
{
    // FIXME: check bounds
    struct list_node *current = *head;

    int i = 0;
    while (current)
    {
        if (i == index)
        {
            return current;
        }

        current = current->next;
        i++;
    }

    return NULL;
}

struct list_node *list_remove(struct list_node **head, struct list_node *item)
{
    struct list_node *current = *head;
    struct list_node *previous = NULL;

    while (current)
    {
        if (current == item)
        {
            if (previous == NULL)
            {
                *head = current->next;
            }
            else
            {
                previous->next = current->next;
            }

            free(current);
            return *head;
        }

        previous = current;
        current = current->next;
    }

    return NULL;
}

struct list_node *list_reverse(struct list_node **head)
{
    struct list_node *current = *head;
    struct list_node *next = NULL;
    struct list_node *prev = NULL;
    while (current != NULL)
    {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    *head = prev;
    return *head;
}