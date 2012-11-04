CFLAGS=	-W -Wall -pthread -g -I../inc
LDFLAGS= -ldl -ljansson -lwebsockets -lz

all:
	$(CC) $(CFLAGS) -DDEBUG ../examples/unnamed_scientist.c headcrab.c headcrab_dispatch.c headcrab_webserver_lws.c headcrab_message_queue.c $(LDFLAGS) $(ADD) -o webserver;


MSVC    = e:/vc6
CL      = $(MSVC)/bin/cl
CLFLAGS = /MD /TC /nologo $(DBG) /W3 /DNO_SSL \
        /I$(MSVC)/include /I.. /Dsnprintf=_snprintf \
        /link /incremental:no /libpath:$(MSVC)/lib /machine:IX86

windows:
	$(CL) /DUSE_WEBSOCKET /DDEBUG ../examples/unnamed_scientist.c headcrab.c ../lib/mongoose/mongoose.c $(CLFLAGS)        

clean:
	rm -rf hello hello.exe upload upload.exe post post.exe websocket websocket.exe chat chat.exe *.dSYM *.obj
