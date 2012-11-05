CFLAGS=	-W -Wall -Wno-unused-parameter -Wno-unused-variable -pthread -g -I../inc
LDFLAGS= -ldl -lz -ljansson -lwebsockets

HC_SRCS = $(wildcard headcrab*.c)
PROG = libheadcrab.a

all:
	$(CC) $(CFLAGS) $(HC_SRCS) $(LDFLAGS) $(ADD) -c

lib: all
	ar rcs $(PROG) headcrab*.o

clean:
	rm -rf $(PROG) *.obj *.o
