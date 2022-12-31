#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>

#define PPCAST(x) (struct list_node **)(x)
#define PCAST(x) (struct list_node *)(x)

/// A node of the linked list
struct list_node
{
    struct list_node *next;
};

/// Retrieves the last element of linked list
struct list_node *list_get_tail(struct list_node **head);

/// Appends a new element to linked list
struct list_node *list_append(struct list_node **head, struct list_node *item);

/// Removes the first element of the linked list.
/// The removed element is returned;
struct list_node *list_pop(struct list_node **head);

/// Returns the index element of the linked list
struct list_node *list_get_node(struct list_node **head, const uint32_t index);

/// Removes item from the linked list
struct list_node *list_remove(struct list_node **head, struct list_node *item);

/// Reverses all the element of the linked list
struct list_node *list_reverse(struct list_node **head);

#endif