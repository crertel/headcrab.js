#include "mongoose.h"
#include "headcrab.h"
#include "headcrab_message_queue.h"

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
		json_error_t error;
		char *buffer = malloc(sizeof(char) * 1024 * 16);

    // This is returning a raw frame
		int len = mg_read(conn, buffer, sizeof(char) * 1024 * 16);

		json_t *message = json_loads(buffer, len, &error);

    LOG_MSG(error.text);


		free(buffer);
    LOG_MSG("Pushing message onto in-queue.\n");
		mq_push(MQ_IN, message);
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