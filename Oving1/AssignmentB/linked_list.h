#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/* Internal node type */
struct node {
	struct node *next;
	struct node *prev;
	int data;
};

/* Internal data type representing list */
struct list {
	struct node *head;
	struct node *tail;
	int length;
};

/* Abstract data type for lists */
typedef struct list *list_t;

/* Create an empty list */
list_t list_create();

/* Delete the list and all internal nodes */
void list_delete(list_t list);

/* Insert data in list at index */
void list_insert(list_t list, int index, int data);

/* Append data to end of list */
void list_append(list_t list, int data);

/* Print data of list in order */
void list_print(list_t list);

/* Get sum of all list elements */
long list_sum(list_t list);

/* Get data at index */
int list_get(list_t list, int index);

/* Extract data at index */
int list_extract(list_t list, int index);

#endif
