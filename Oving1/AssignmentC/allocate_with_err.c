#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>

void allocate() {
    uint8_t  *ptr = NULL, value;
    ptr = (uint8_t *)malloc(0x1000000 * sizeof(uint8_t));
    if (!ptr) {
    	errno = -1;
    }
    else {
	    *ptr = value;
	    printf("test of allocated memory: %" PRIu8 "\n", value);
	}
}


int main() {
    while (1) {
        allocate();
        if (errno != 0) { 
            perror("Could not allocate\n");
            return; 
        }
    }

    return 0;
}
