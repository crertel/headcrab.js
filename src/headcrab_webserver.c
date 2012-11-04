#include "mongoose.h"
#include "headcrab.h"

struct mg_context * ctx;

static void * websocket_message_handler(enum mg_event event,
						  		 struct mg_connection *conn)
{
	if (event == MG_WEBSOCKET_CONNECT) {
		LOG_MSG("Client connecting...\n");
	} else if (event == MG_WEBSOCKET_READY) {
		LOG_MSG("Server ready.\n");
	} else if (event == MG_WEBSOCKET_MESSAGE) {
		LOG_MSG("Received message.\n");
	} else if (event == MG_WEBSOCKET_CLOSE) {
		LOG_MSG("Server closed by client.\n");
	}

	return NULL;
}

int websocket_initialize(const char * _assetDir)
{
	// Set options.
	const char * options[] = {
		"listening_ports", "8080",
		"document_root", _assetDir,
		NULL
	};
	// Start web server.
	ctx = mg_start(&websocket_message_handler, NULL, options);
	if (!ctx)
		return -1;
	return 0;
}

void websocket_shutdown()
{
	mg_stop(ctx);
}