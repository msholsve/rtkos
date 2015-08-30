# Variables
SHELL= /bin/bash
CP= cp
CPP= cpp
GUNZIP= gunzip
LD= ld
MAKE= make
RM= rm
SED= sed
TAR= tar
TRUE= true


# Compiler warnings
CWARN= -Wall -pedantic

# Compiler standard
CSTD= -std=c99

# Optimization
OPT= -O2

# Libraries need to build
LIBS= -pthread

# Default flags
CFLAGS= ${CWARN} ${CSTD} ${OPT} \
	-Wno-comment -Wno-dangling-else -Wno-implicit-function-declaration \
    -Wno-incompatible-pointer-types -Wno-parentheses \
    -Wno-trigraphs --include string.h

# Compiler
CC= cc
MAY_NEED_GCC= gcc

# Special flags for this
ENTRY= prog
DATA= 
ALT_OBJ=
ALT_ENTRY=

# Build the entry
all: ${ENTRY} ${DATA}
	@${TRUE}

${ENTRY}: ${ENTRY}.c
	${CC} ${CFLAGS} $< -o $@ ${LIBS}

clean:
	${RM} -f ${ENTRY} ${ENTRY}.o ${ALT_OBJ}