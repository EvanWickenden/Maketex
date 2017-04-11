
CC = gcc

CFLAGS = -g -Wall -D__EDITOR__=\"$(EDITOR)\" $(LOG) 
LDFLAGS = -g 

# replace 'vim' with choice of editor
EDITOR = vim

DIR = /usr/local/bin

# leave empty 
LOG =

.PHONY: default clean log install

default: maketex

maketex:

log: LOG=-D__LOG__
log: clean maketex

clean:
	rm -f maketex{,.o,} *.log
	rm -rf maketex.dSYM

install: default
	cp -f ./maketex $(DIR)


