ifdef VERBOSE
	Q = 
	E = @true 
else
	Q = @
	E = @echo 
endif

CFILES := $(shell find src -mindepth 1 -maxdepth 4 -name "*.c")

INCLUDE := inc

OBJFILES := $(CFILES:src/%.c=%)
OFILES := $(OBJFILES:%=obj/%.o)

BINFILE = oving7

XENO_DESTDIR :=
XENO_CONFIG := $(XENO_DESTDIR)/usr/bin/xeno-config
XENO_CFLAGS := $(shell DESTDIR=$(XENO_DESTDIR) $(XENO_CONFIG) --skin native --cflags)
XENO_LIBS := $(shell DESTDIR=$(XENO_DESTDIR) $(XENO_CONFIG) --skin native --ldflags)

# NO -pedantic!!
COMMONFLAGS = -Wall -Wextra 
LIBS = -lm -lrt -pthread $(XENO_LIBS)
DEFINES = 

ifdef DEBUG
	COMMONFLAGS := $(COMMONFLAGS) -g
endif

# NO -std=c99!!!
CFLAGS = $(COMMONFLAGS) $(XENO_CFLAGS)

all: $(BINFILE)

CC = gcc

.PHONY: clean all
.SUFFIXES:

obj/%.o: src/%.c
	$(E)C-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CC) -o $@ -c $< -I$(INCLUDE) $(CFLAGS) $(DEFINES)

$(BINFILE): $(OFILES)
	$(E)Linking $@
	$(Q)$(CC) -o bin/$@ $(OFILES) $(LIBS)

clean:
	$(E)Removing files
	$(Q)rm -f obj/* bin/*