#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>

void allocate(uint64_t value) {
    uint64_t  *ptr = NULL;
    ptr = (uint64_t *)malloc(100000000 * sizeof(uint64_t));
    if (!ptr) {
    	errno = -1;
    }
    else {
	    *ptr = value;
	    printf("test of allocated memory: %" PRIu64 "\n", value);
	}
}


int main() {
    while (1) {
        allocate(0xfffffffffffffff);
        if (errno != 0) { 
            perror("Could not allocate\n");
            return; 
        }
    }

    return 0;
}
