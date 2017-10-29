
CC = gcc


CFLAGS = -Wall -Werror -Wpedantic -D__EDITOR__=\"$(EDITOR)\" -D__SCRIPT_DIR__=\"$(CDIR)\" $(LOG) 
LDFLAGS = 

# replace 'vim' with choice of editor
EDITOR = vim

# replace with prefered installation directory
DIR = /usr/local/bin

# directory containing maketex.sh -- this directory by default
CDIR = $(shell pwd)

# leave empty 
LOG =

.PHONY: default clean log install touch

default: maketex

maketex:

log: LOG=-D__LOG__
log: touch maketex

touch:
	touch maketex.c

clean:
	rm -f maketex{,.o,} *.log
	rm -rf maketex.dSYM

install: default
	cp maketex $(DIR)
#	ln -f ./maketex $(DIR)


