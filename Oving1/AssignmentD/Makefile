# program executable name 
TARGET = time_testmake 

# compiler flags 
CFLAGS = -g -Wall -std=c99

# linker flags 
LDFLAGS = -g -lrt

# list of sources 
SOURCES = $(shell find -name "*.c") 

# default rule, to compile everything 
all: $(TARGET) 

# define object files 
OBJECTS = $(SOURCES:.c=.o) 

# link programs 
$(TARGET): $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^ -lpthread

# compile 
%.o : %.c 
	gcc $(CFLAGS) -c -o $@ $< 

# cleaning 
clean: 
	rm -f $(TARGET) $(OBJECTS) 