/*
 * libwebsockets-test-server - libwebsockets test implementation
 *
 * Copyright (C) 2010-2011 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#include "headcrab.h"
#include "headcrab_webserver_lws.h"
#include "headcrab_message_queue.h"
#include <libwebsockets.h>

static struct libwebsocket_context *context;
static pthread_t tid;
static int running = 1;

enum demo_protocols {
	/* always first */
	PROTOCOL_HTTP = 0,

	PROTOCOL_COMMAND,

	/* always last */
	DEMO_PROTOCOL_COUNT
};


#define LOCAL_RESOURCE_PATH "./"

/* this protocol server (always the first one) just knows how to do HTTP */

static int callback_http(struct libwebsocket_context *context,
		struct libwebsocket *wsi,
		enum libwebsocket_callback_reasons reason, void *user,
							   void *in, size_t len)
{
	char client_name[128];
	char client_ip[128];

	switch (reason) {
	case LWS_CALLBACK_HTTP:
		fprintf(stderr, "serving HTTP URI %s\n", (char *)in);

		if (in && strcmp(in, "/favicon.ico") == 0) {
			if (libwebsockets_serve_http_file(wsi,
			     LOCAL_RESOURCE_PATH"/favicon.ico", "image/x-icon"))
				fprintf(stderr, "Failed to send favicon\n");
			break;
		}

		/* send the script... when it runs it'll start websockets */

		if (libwebsockets_serve_http_file(wsi,
				  LOCAL_RESOURCE_PATH"/test.html", "text/html"))
			fprintf(stderr, "Failed to send HTTP file\n");
		break;

	/*
	 * callback for confirming to continue with client IP appear in
	 * protocol 0 callback since no websocket protocol has been agreed
	 * yet.  You can just ignore this if you won't filter on client IP
	 * since the default uhandled callback return is 0 meaning let the
	 * connection continue.
	 */

	case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:

		libwebsockets_get_peer_addresses((int)(long)user, client_name,
			     sizeof(client_name), client_ip, sizeof(client_ip));

		fprintf(stderr, "Received network connect from %s (%s)\n",
							client_name, client_ip);

		/* if we returned non-zero from here, we kill the connection */
		break;

	default:
		break;
	}

	return 0;
}

/* command protocol */

/*
 * one of these is auto-created for each connection and a pointer to the
 * appropriate instance is passed to the callback in the user parameter
 *
 * for this example protocol we use it to individualize the count for each
 * connection.
 */

static int
callback_command(	struct libwebsocket_context *context,
					struct libwebsocket *wsi,
					enum libwebsocket_callback_reasons reason,
					void *user, void *in, size_t len)
{
	int n;

	switch (reason) {

	case LWS_CALLBACK_ESTABLISHED:
		LOG_MSG("command: LWS_CALLBACK_ESTABLISHED\n");
		break;

	case LWS_CALLBACK_RECEIVE:
		LOG_MSG("Received message len %d\n", (int)len);
		json_error_t error;

		// we aren't writing today
		//char *buffer = malloc(LWS_SEND_BUFFER_PRE_PADDING + len +
		//				  LWS_SEND_BUFFER_POST_PADDING);
		//unsigned char *p = &buffer[LWS_SEND_BUFFER_PRE_PADDING];

		json_t *message = json_loads(in, len, &error);

		// print more informative error
    	LOG_ERROR(error.text);


    	LOG_MSG("Pushing message onto in-queue.\n");
		mq_push(MQ_IN, message);
		break;
	default:
		break;
	}

	return 0;
}


/* list of supported protocols and callbacks */

static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */

	{
		"http-only",		/* name */
		callback_http,		/* callback */
		0			/* per_session_data_size */
	},
	{
		"command",
		callback_command,
		0,
	},
	{
		NULL, NULL, 0		/* End of list */
	}
};

void* websocket_thread(void *arg)
{
	while (running)
	{
		libwebsocket_service(context, 50);
	}
	libwebsocket_context_destroy(context);

	return NULL;
}

int websocket_initialize(const char * _assetDir)
{
	int n = 0;
	const char *cert_path = NULL;
	const char *key_path = NULL;
	int port = 8080;
	int opts = 0;
	const char *interface = NULL;


	context = libwebsocket_create_context(port, interface, protocols,
				libwebsocket_internal_extensions,
				cert_path, key_path, -1, -1, opts, NULL);
	if (context == NULL) {
		LOG_ERROR("libwebsocket init failed\n");
		return -1;
	}

	pthread_create(&tid, NULL, websocket_thread, MQ_IN);

	return 0;
}

void websocket_shutdown()
{
	running = 0;
	pthread_join(tid, NULL);	
}