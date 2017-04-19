
CC = gcc

CFLAGS = -g -Wall -Werror -Wpedantic -D__EDITOR__=\"$(EDITOR)\" $(LOG) 
LDFLAGS = -g 

# replace 'vim' with choice of editor
EDITOR = vim

# replace with prefered installation directory
DIR = /usr/local/bin

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
	ln -f ./maketex $(DIR)


