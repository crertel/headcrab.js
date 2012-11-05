#include "headcrab.h"
#include "headcrab_webserver_lws.h"
#include "headcrab_dispatch.h"
#include "headcrab_message_queue.h"
#include <string.h>

HC_LogFunction log_call = printf;

HEADCRAB_ERROR headcrab_init(const char* _assetDir)
{
	// Default logging goes to stdout.
	//log_call = printf;
	// Initialize the dispatch table.
	dispatch_table_init();
	// Start Websocket Server.

	LOG_MSG("Starting server.\n");
	if (websocket_initialize(_assetDir) == 0) {
		LOG_MSG("Server started.\n");
	} else {
		LOG_ERROR("Failed to start server.\n");
		headcrab_shutdown();
		return HC_FAIL;
	}

	return HC_SUCCESS;
}

void headcrab_shutdown()
{
	LOG_MSG("Server shutdown.\n");
	websocket_shutdown();
	return;
}

void headcrab_set_logging_callback( HC_LogFunction _callback)
{
	log_call = _callback;
}

void headcrab_bind_object(  void* _target,
                            const char* _name,
                            const char* _verb,
                            HC_PreOpFunction _preOp,
                            const void* _preOpArgs,
                            HC_MutatorFunction _op,
                            HC_PostOpFunction _postOp,
                            const void* _postOpArgs
                        )
{
	// We don't allow null verbs.
	if (_verb == NULL)
	{
		return;
	}
	LOG_MSG("Binding: %s to %s.\n", _name, _verb);
	// Copy the strings.
	char * name = malloc((strlen(_name) + 1) * sizeof(*_name));
	strcpy(name, _name);
	char * verb = malloc((strlen(_verb) + 1) * sizeof(*_verb));
	strcpy(verb, _verb);
	// Add to dispatch table.
	dispatch_table_add(_target, name,
						verb, _preOp, _preOpArgs,
						_op, _postOp, _postOpArgs);
	return;
}

void headcrab_clear_all_object_bindings( const char* _name )
{
	// Cannot delete the global namespace node.
	if (*_name == '\0')
		return;
	dispatch_table_remove(_name);
}

void headcrab_rebind_object( const char* _name, void* _object)
{
	dispatch_table_rebind(_name, _object);
}

void dispatch(json_t *message)
{
	if (!json_is_object(message))
	{
		LOG_ERROR("JSON message is not an object!\n");
		return;
	}
	
	json_t *type, *target, *command, *args, *seqID;

	type = json_object_get(message, "type");
	if (!json_is_string(type) || strcmp(json_string_value(type), "command"))
	{
		LOG_ERROR("JSON type is not a string!\n");
		return;
	}

	target = json_object_get(message, "target");
	if (!json_is_string(target))
	{
		LOG_ERROR("JSON target is not a string!\n");
		return;
	}

	command = json_object_get(message, "command");
	if (!json_is_string(command))
	{
		LOG_ERROR("JSON command is not a string!\n");
		return;
	}

	args = json_object_get(message, "args");
	json_incref(args);

	seqID = json_object_get(message, "seqID");
	if (!json_is_integer(seqID))
	{
		LOG_ERROR("JSON seqID is not an integer!\n");
		return;
	}

	LOG_MSG("Dispatching on message.\n");
	json_object_set(args, "seqID", seqID);
	dispatch_table_execute( json_string_value(target),
							json_string_value(command),
							args);

	json_decref(message);
}

void headcrab_handle_commands()
{
	json_t *command = (json_t *)mq_pop(MQ_IN);

	for(; NULL != command; command = (json_t *)mq_pop(MQ_IN))
	{
		LOG_MSG("Pulling message off of in-queue.\n");
		dispatch(command);
	}
}

void headcrab_post_message( int _seqID, json_t* _message)
{
	//json_t* wrapper = json_object();s
	//json_object_set(wrapper, "data", _message);
	json_object_set_new(_message, "seqID", json_integer(_seqID));

	LOG_MSG("Pushing message onto out-queue.\n");
	mq_push(MQ_OUT, wrapper);
	websocket_notify();
}