#include "mongoose.h"

struct mg_context * ctx;

static void * ws_message_handler(enum mg_event event,
						  		 struct mg_connection *conn)
{
	if (event == MG_WEBSOCKET_READY) {
		MSG_LOG("Server ready.\n");
	} else if (event == MG_WEBSOCKET_MESSAGE) {
		MSG_LOG("Received message.\n");
	} else if (event == MG_WEBSOCKET_DUNNO) {

	}

	return NULL;
}

int websocket_initialize(char * _assetDir)
{
	// Set options.
	const char * options[] = {
		"listening_ports", "8080",
		"document_root", _assetDir,
		NULL
	};
	// Start web server.
	ctx = mg_start(&ws_message_handler, NULL, options);
	if (!ctx)
		return -1;
	return 0;
}

void websocket_shutdown()
{
	mg_stop(ctx);
}