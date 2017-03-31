
CC = gcc

CFLAGS = -g -Wall $(EDITOR) $(LOG) 
LDFLAGS = -g 

LOG =

# replace 'vim' with choice of editor
EDITOR = -D __EDITOR__=\"vim\"

DIR = /usr/local/bin

.PHONY: default clean log install

default: maketex

maketex:

log: DEF = __LOG__=1
log: clean maketex

clean:
	rm -f maketex{,.o,} *.log
	rm -rf maketex.dSYM

install: default
	cp -f ./maketex $(DIR)


