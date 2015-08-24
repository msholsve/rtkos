#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

list_t list_create()
{
	return (list_t)calloc(1, sizeof(struct list));
}

void list_delete(list_t list)
{	
	while (list->head) 
	{
		struct node *tmp = list->head;
		list->head = list->head->next;
		free(tmp);
	}
	free(list);
}

void list_insert(list_t list, int index, int data)
{

	if (index > list->length) {
		return;
	}
	

	if (list->length == 0) {
		return list_append(list, data);
	}

	struct node *newNode = (struct node *)calloc(1, sizeof(struct node));
	newNode->data = data;

	if (index == 0) {
		newNode->next = list->head;
		list->head->prev = newNode;
		list->head = newNode;
		list->length++;
		return;
	}

	struct node *pCurrNode = list->head;
	for (int i = 0; i < index - 1; i++)
	{
		if (!pCurrNode) {
			printf("FML\n");
			free(newNode);
			return;
		}
		pCurrNode = pCurrNode->next;
	}

	newNode->next = pCurrNode->next;
	newNode->prev = pCurrNode;
	if (pCurrNode->next) {
		pCurrNode->next->prev = newNode;
	}
	else {
		list->tail = newNode;
	}
	pCurrNode->next = newNode;
	list->length++;
}

void list_append(list_t list, int data)
{
	struct node *newNode = (struct node *)calloc(1, sizeof(struct node));
	newNode->data = data;

	list->length++;
	if (!list->head) {
		list->head = newNode;		
		list->tail = newNode;
		return;
	}

	list->tail->next = newNode;
	newNode->prev = list->tail;
	list->tail = newNode;
}

void list_print(list_t list)
{
	printf("list: ");
	struct node *tmp = list->head;
	while (tmp)
	{
		printf("%i ", tmp->data);
		tmp = tmp->next;
	}
	printf("\n");
}

long list_sum(list_t list)
{
	long summa = 0;
	struct node *tmp = list->head;
	while (tmp)
	{
		summa += tmp->data;
		tmp = tmp->next;
	}
	return summa;
}

int list_get(list_t list, int index)
{
	struct node *tmp = list->head;
	while (tmp && index--)
	{
		tmp = tmp->next;
	}

	return tmp ? tmp->data : 0x00;
}

int list_extract(list_t list, int index)
{
	if (index > list->length - 1) {
		return 0;
	}

	int currIndex = index;
	struct node *tmp = list->head;
	while (tmp && currIndex--)
	{
		tmp = tmp->next;
	}
	if (!tmp) return 0;

	int returnVal = tmp->data;
	list->length--;

	// head and tail
	if (index == 0) {
		list->head = list->head->next;
	}
	if (index == list->length - 1) {
		list->tail = list->tail->prev;
	}
	// node correctness
	if (tmp->next) {
		tmp->next->prev = tmp->prev;
	}
	if (tmp->prev) {
		tmp->prev->next = tmp->next;		
	}

	free(tmp);
	return returnVal;
}
