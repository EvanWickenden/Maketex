
CC=gcc

CFLAGS=-g -Wall $(DEF) 
LDFLAGS=-g 

DEF=

DIR=/usr/local/bin

.PHONY: default clean log install

default: maketex

maketex:

log: DEF = -D __LOG__=1
log: clean maketex

clean:
	rm -f maketex{,.o,} *.log
	rm -rf maketex.dSYM

install: default
	sudo cp -f ./maketex $(DIR)


