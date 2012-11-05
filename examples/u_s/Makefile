CFLAGS=	-W -Wall -Wno-unused-parameter -pthread -Wno-unused-variable -g -I../../inc -L../../src
LDFLAGS= -lheadcrab -ljansson -lwebsockets

SRCS = $(wildcard *.c)
PROG = u_s

all:
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) $(ADD) -o $(PROG)

clean:
	rm -rf $(PROG) *.obj *.o
