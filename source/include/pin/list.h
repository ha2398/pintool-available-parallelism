/*
 * list.h
 */

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

/*
 * node - defines a single node in a linked list.
 * @data: pointer to the data in the node.
 * @next: point to the next node in the list.
 */
 typedef struct node {
 	void *data;
 	struct node *next;
 } node;

/*
 * list - defines a singly linked list.
 * @length: stores the amount of nodes in the list.
 * @head: pointer to the first node in the list.
 */
 typedef struct list {
 	unsigned long length;
 	struct node *head;
 } list;

/*
 * new_list - returns a pointer to an empty list.
 */
list *new_list();

/* is_list_empty - returns true iff the list is empty.
 * @lp: pointer to list to check.
 */
bool is_list_empty(list *lp);

/*
 * list_add - adds a new node in a list and returns a pointer to it.
 * Returns NULL in failure.
 * @lp: pointer to the list to add the node in.
 * @data: pointer to the data the new node will hold.
 */
node *list_add(list *lp, void *data);

/*
 * list_contains - returns true iff the list contains a node which holds
 * @data.
 * @lp: pointer to the list to check.
 * @data: pointer to the data to check.
 * @equals: function that returns true iff the content of two
 * void pointers are the same.
 */
bool list_contains(list *lp, const void *data,
	bool (*equals)(const void *, const void *));

/*
 * delete_list - deletes all nodes in the list.
 * @lp: pointer to list to delete.
 */
void delete_list(list *lp);

/*
 * list_size - returns the number of elements in the list.
 * @lp: pointer to the list to check size.
 */
unsigned long list_size(list *lp);

#endif /* LIST_H */