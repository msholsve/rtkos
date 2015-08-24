#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include "linked_list.h"

int main(int argc, char **argv)
{
	int i;
	list_t list;

	printf("create list\n");
	list = list_create();

	// append the values 0 to 9 to the list
	printf("append valued 0 to 9\n");
	for(i=0; i<10; i++)
	{
		list_append(list, i);
		list_print(list);
	}

	// print sum of all values in the list
	printf("list sum: %li\n", list_sum(list));

	// insert value in the middle of the list
	printf("insert value in the middle of the list\n");
	list_insert(list, 5, 99);
	list_print(list);

	// get inserted value
	printf("get inserted value: %i\n", list_get(list, 5));
	list_print(list);

	// extract inserted value
	printf("extract inserted value: %i\n", list_extract(list, 5));
	list_print(list);

	// remove all but the last values from list
	printf("remove all but the last value\n");
	while(list->length > 1)
	{
		list_extract(list, 0);
		list_print(list);
	}

	// remote the last value from the list
	printf("remove the last value\n");
	list_extract(list, 0);
	list_print(list);

	// delete list
	printf("delete list\n");
	list_delete(list);

	return 0;
}
