CFLAGS=	-W -Wall -Wno-unused-parameter -Wno-unused-variable -DDEBUG -pthread -g -L../../src -I../../inc
LDFLAGS= -lheadcrab -ldl -ljansson -lwebsockets -lz -lm

SRCS = src/simulate.c src/boid.c
PROG = boids_server

all:
	$(CC) $(CFLAGS) -DDEBUG $(SRCS) $(HC_SRCS) $(LDFLAGS) $(ADD) -o $(PROG);

clean:
	rm -rf $(PROG) *.dSYM *.obj
