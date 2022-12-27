#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#define PPCAST(x) (struct list_node **)(x)
#define PCAST(x) (struct list_node *)(x)

struct list_node
{
    struct list_node *next;
};

struct list_node *list_get_tail(struct list_node **head);

struct list_node *list_append(struct list_node **head, struct list_node *item);

struct list_node *list_pop(struct list_node **head);

struct list_node *list_get_node(struct list_node **head, const unsigned int index);

struct list_node *list_remove(struct list_node **head, struct list_node *item);

struct list_node *list_reverse(struct list_node **head);

#endif