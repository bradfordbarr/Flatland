CC=gcc
CFLAGS=-c -g -Wall -ansi
INCLUDES=$(shell guile-config compile)
LDFLAGS=$(shell guile-config link)
EXE=flatland
OFILES=cell.o board.o main.o

all: flatland

flatland: ${OFILES}
	${CC} ${INCLUDES} ${LDFLAGS} ${OFILES} -o ${EXE}

main.o:
	${CC} ${INCLUDES} ${CFLAGS} main.c

board.o:
	${CC} ${INCLUDES} ${CFLAGS} board.c

cell.o:
	${CC} ${INCLUDES} ${CFLAGS} cell.c

.PHONY: clean

clean:
	rm -rf *.o ${EXE}
