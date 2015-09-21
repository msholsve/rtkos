ifdef VERBOSE
	Q = 
	E = @true 
else
	Q = @
	E = @echo 
endif

CFILES := $(shell find src -mindepth 1 -maxdepth 4 -name "*.c")

INCLUDE := inc

OBJFILES := $(CXXFILES:src/%.cpp=%) $(CFILES:src/%.c=%)
OFILES := $(OBJFILES:%=obj/%.o)

BINFILE = oving5

COMMONFLAGS = -Wall -Wextra -pedantic
LDFLAGS = -lcomedi -lm -lrt -pthread
DEFINES = -D_GNU_SOURCE

ifdef DEBUG
	COMMONFLAGS := $(COMMONFLAGS) -g
endif
CFLAGS = $(COMMONFLAGS) --std=c99

all: $(BINFILE)

CC = gcc

.PHONY: clean all
.SUFFIXES:

obj/%.o: src/%.c
	$(E)C-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CC) -o $@ -c $< -I $(INCLUDE) $(CFLAGS) $(DEFINES)

$(BINFILE): $(OFILES)
	$(E)Linking $@
	$(Q)$(CC) -o bin/$@ $(OFILES) $(LDFLAGS)

clean:
	$(E)Removing files
	$(Q)rm -f obj/* bin/*