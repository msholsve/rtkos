#ifndef FIFO__H

#include <pthread.h>

#define TRUE 1
#define FALSE 0

//fifo parameters
#define LENGTH 4
#define WIDTH 255

struct fifo_t;
typedef struct fifo_t *fifoHandler_t;

fifoHandler_t init_fifo(void);

//adds a string to the fifo
int fifo_add_string(fifoHandler_t fifo,char array[WIDTH]);
//copys the next string into "string" and removes it from the fifo
void fifo_rem_string(fifoHandler_t fifo, char * string);

//adds a id to the blocked list
int fifo_add_blocked_id(fifoHandler_t fifo, int id);
//returns the next id from the blocked list
int fifo_rem_blocked_id(fifoHandler_t fifo);

//returns the status of the fifo, 0 = empty, 1 = non-empty
int fifo_status(fifoHandler_t fifo);

//prints the fifo list
void fifo_print(fifoHandler_t fifo);

//prints the blocked id list
void fifo_print_blocked_ids(fifoHandler_t fifo);

#endif
