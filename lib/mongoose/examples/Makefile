CFLAGS=	-W -Wall -I.. -pthread -g
LDFLAGS= -ldl

all:
	$(CC) $(CFLAGS) hello.c ../mongoose.c  $(LDFLAGS) $(ADD) -o hello;
	$(CC) $(CFLAGS) upload.c ../mongoose.c $(LDFLAGS) $(ADD) -o upload;
	$(CC) $(CFLAGS) post.c ../mongoose.c $(LDFLAGS) $(ADD) -o post;
	$(CC) $(CFLAGS) -DUSE_WEBSOCKET websocket.c ../mongoose.c $(LDFLAGS) $(ADD) -o websocket;
	$(CC) $(CFLAGS) chat.c ../mongoose.c $(LDFLAGS) $(ADD) -o chat


MSVC    = e:/vc6
CL      = $(MSVC)/bin/cl
CLFLAGS = /MD /TC /nologo $(DBG) /W3 /DNO_SSL \
        /I$(MSVC)/include /I.. /Dsnprintf=_snprintf \
        /link /incremental:no /libpath:$(MSVC)/lib /machine:IX86

windows:
	$(CL) hello.c ../mongoose.c $(CLFLAGS)
	$(CL) upload.c ../mongoose.c $(CLFLAGS)
	$(CL) post.c ../mongoose.c $(CLFLAGS)
	$(CL) post.c ../mongoose.c $(CLFLAGS)
	$(CL) /DUSE_WEBSOCKET websocket.c ../mongoose.c $(CLFLAGS)

clean:
	rm -rf hello hello.exe upload upload.exe post post.exe websocket websocket.exe chat chat.exe *.dSYM *.obj
