CFLAGS=	-W -Wall -Wno-unused-parameter -Wno-unused-variable -pthread -g -Iinc
LDFLAGS= -ldl -ljansson -lwebsockets -lz

HC_SRCS = $(wildcard src/headcrab*.c)
SRCS = examples/unnamed_scientist.c
PROG = webserver

all:
	$(CC) $(CFLAGS) -DDEBUG $(SRCS) $(HC_SRCS) $(LDFLAGS) $(ADD) -o $(PROG);

clean:
	rm -rf $(PROG) *.dSYM *.obj
