#include "headcrab.h"
#include "headcrab_dispatch.h"

HC_ObjectNode * dispatch_table;

HEADCRAB_ERROR headcrab_init(const char* _assetDir)
{
	// Default logging goes to stdout.
	log_call = printf;
	// Initialize the dispatch table.
	dispatch_table = malloc(sizeof(*new_node));
	dispatch_table = { "", NULL, NULL, NULL };
	// Start Websocket Server.
	LOG_MSG("Connecting to server.");
	if ( websocket_initialize(_assetDir) ) {
		LOG_MSG("Connected to server.");
	} else {
		LOG_ERROR("Failed to connect to server.");
		headcrab_shutdown();
	}
}

void headcrab_shutdown()
{
	LOG_MSG("Disconnecting from server.");
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
	size_t name_len = strnlen(_name, MAXSTRLEN);
	char * name = malloc(name_len * sizeof(*_name));
	strncpy(name, _name, MAXSTRLEN);
	size_t verb_len = strnlen(_verb, MAXSTRLEN);
	char * verb = malloc(verb_len * sizeof(*_verb));
	strncpy(verb, _verb, MAXSTRLEN);
	// Add to dispatch table.
	dispatch_table_add(dispatch_table, _target, name,
						verb, _preOp, _preOpArgs,
						_op, _postOp, _postOpArgs);
	return;
}

void headcrab_clear_all_object_bindings( const char* _name )
{
	// Cannot delete the global namespace node.
	if (*name == '\0')
		return;
	dispatch_table_remove(dispatch_table, _name);
}

void headcrab_rebind_object( const char* _name, void* _object)
{
	dispatch_table_rebind(dispatch_table, _name, _object);
}