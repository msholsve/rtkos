#######################
###   Miniproject   ###
#######################

# Path for buildroot
BUILD_DIR := /home/student

# Div paths for building
KERNELDIR := $(BUILD_DIR)/buildroot-avr32/output/build/linux-2.6.35.4
CROSS := $(BUILD_DIR)/buildroot-avr32/output/staging/usr/bin/avr32-linux-

CFILES := $(shell find src -mindepth 1 -maxdepth 4 -name "*.c")

INCLUDE := -Iinc

OBJFILES := $(CFILES:src/%.c=%)
OFILES := $(OBJFILES:%=obj/%.o)

PROGRAM = controller

CC = gcc
MAKE = make

CFLAGS = -g -Wall -Wextra -pedantic --std=gnu99
LDFLAGS = -pthread -lrt
DEFINES = 

all:
	$(MAKE) $(PROGRAM) 2>&1 | ./colorize

cross:
	$(MAKE) CC=$(CROSS)gcc

.PHONY: clean all cross
.SUFFIXES:

obj/%.o: src/%.c
	@echo C-compiling $<
	@if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) -o $@ -c $< $(INCLUDE) $(CFLAGS) $(DEFINES)

$(PROGRAM): $(OFILES)
	@echo Linking $@
	$(CC) -o bin/$@ $(OFILES) $(LDFLAGS)

clean:
	@echo Removing files
	rm -f obj/* bin/*
