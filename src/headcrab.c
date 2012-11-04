#include "headcrab.h"
#include "headcrab_dispatch.h"

HEADCRAB_ERROR headcrab_init(const char* _assetDir)
{
	// Default logging goes to stdout.
	log_call = printf;
	// Initialize the dispatch table.
	dispatch_table_init();
	// Start Websocket Server.
	LOG_MSG("Connecting to server.\n");
	if ( websocket_initialize(_assetDir) ) {
		LOG_MSG("Connected to server.\n");
	} else {
		LOG_ERROR("Failed to connect to server.\n");
		headcrab_shutdown();
	}
}

void headcrab_shutdown()
{
	LOG_MSG("Disconnecting from server.\n");
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
                            HC_PreOpFucntion _preOp,
                            const void* _preOpArgs,
                            HC_MutatorFunction _op,
                            HC_PostOpFucntion _postOp,
                            const void* _postOpArgs
                        )
{
	// Copy the strings.
	char * name = malloc(strlen(_name) * sizeof(*_name));
	strcpy(name, _name);
	char * verb = malloc(strlen(_verb) * sizeof(*_verb));
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
	if (*name == '\0')
		return;
	dispatch_table_remove(_name);
}

void headcrab_rebind_object( const char* _name, void* _object)
{
	dispatch_table_rebind(_name, _object);
}