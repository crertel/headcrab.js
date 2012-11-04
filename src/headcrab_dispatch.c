#include "headcrab_dispatch.h"

static HC_ObjectNode * dispatch_table;

void dispatch_table_init()
{
	HC_ObjectNode * new_node;
	dispatch_table = malloc(sizeof(*new_node));
	if (NULL == dispatch_table)
	{
		LOG_ERROR("Could not allocate new HC_ObjectNode in dispatch_table_init.\n");
		exit(0);
	}
	*dispatch_table = (HC_ObjectNode){ "", NULL, NULL, NULL };
}

/*
	If there is a node with the name _name in dispatch_table, rebinds the node 
	to use _target as its object. If there is no such node, does nothing.
*/
void dispatch_table_rebind(const char* _name, void* _target)
{
	for(; dispatch_table != NULL; dispatch_table++)
	{
		if (0 == strcmp(dispatch_table->name, _name))
		{
			dispatch_table->object = _target;
		}
	}
}

/*
	Removes the node with the name _name from the table, including all handlers
	for that name.

	Pre: _name is not the name of the first object node in the table
*/
void dispatch_table_remove(const char* _name)
{
	HC_ObjectNode *prev_node, *cur_node;
	prev_node = NULL;
	cur_node = dispatch_table;

	for(; cur_node != NULL; cur_node = cur_node->next)
	{
		if (0 == strcmp(cur_node->name, _name))
		{
			// first table element is global object, so prev_node is never null
			prev_node->next = cur_node->next;
			free_node(cur_node);
			break;
		}
	}
}

/*
	Frees all the memory used by node and its related handlers.
*/
void free_node(HC_ObjectNode* _node)
{
	HC_Handler *next_handler, *cur_handler;
	next_handler = cur_handler = _node->handler;

	while(next_handler != NULL)
	{
		next_handler = cur_handler->next;
		free(cur_handler->name);
		free(cur_handler);
	}

	free(_node->name);
	free(_node);
}

/*
	Adds the entry to the dispatch table. If a different object node already 
	using that name is found, returns HC_FAIL. Otherwise, allocates a new node,
	adds it to the list, sets out to the pointer, and returns HC_SUCCESS.
*/
void dispatch_table_add(	void* _target,
	                        const char* _name,
	                        const char* _verb,
	                        HC_PreOpFunction _preOp,
	                        const void* _preOpArgs,
	                        HC_MutatorFunction _op,
	                        HC_PostOpFunction _postOp,
	                        const void* _postOpArgs
							)
{
	HC_ObjectNode* node;

	add_or_find_node(&node, _target, _name);
	node_add_handler(node, _verb, _preOp, _preOpArgs, _op, _postOp, _postOpArgs);
}

/*
	Tries to find the object node in the table. If it is found, sets out to the
	pointer. If an object node already using that name is found, returns
	HC_FAIL. Otherwise, allocates a new node, adds it to the list, sets out to
	the pointer, and returns HC_SUCCESS.
*/
void add_or_find_node(HC_ObjectNode** out,
								void* _target,
								const char* _name
					 			)
{
	HC_ObjectNode *prev_node, *cur_node, *new_node;

	// Search for the target's entry
	prev_node = NULL;
	cur_node = dispatch_table;
	while(NULL != cur_node)
	{
		if (0 == strcmp(cur_node->name, _name))
		{
			// We found a match!
			break;
		}
		prev_node = cur_node;
		cur_node = cur_node->next;
	}

	if (NULL == cur_node)
	{
		new_node = malloc(sizeof(HC_ObjectNode));
		if (new_node == NULL)
		{
			LOG_ERROR("Unable to malloc HC_ObjectNode in add_or_find_node.\n");
			exit(0);
		}

		*new_node = (HC_ObjectNode){_name, NULL, NULL, _target};

		prev_node->next = new_node;

		cur_node = new_node;
	}

	*out = cur_node;
}

HC_ObjectNode* find_node(const char* _name)
{
	HC_ObjectNode* node = dispatch_table;
	for(; NULL != node; node = node->next)
	{
		if (0 == strcmp(node->name, _name))
		{
			return node;
		}
	}
	return NULL;
}


/*
	Adds a new handler for the specified verb to the node.
*/
void node_add_handler(	HC_ObjectNode* node,
                        const char* _verb,
                        HC_PreOpFunction _preOp,
                        const void* _preOpArgs,
                        HC_MutatorFunction _op,
                        HC_PostOpFunction _postOp,
                        const void* _postOpArgs
                      )
{
	HC_Handler *handler, *new_handler;

	// Make the new handler
	new_handler = malloc(sizeof(HC_Handler));
	if (NULL == new_handler)
	{
		LOG_ERROR("Unable to malloc HC_Handler in node_add_handler.\n");
		exit(0);
	}

	*new_handler = (HC_Handler){_verb, NULL, _preOp, _postOp, _op, _preOpArgs, _postOpArgs };

	if (NULL == node->handler)
	{
		// If this will be the first handler, replace node->handler
		node->handler = new_handler;
	}
	else
	{
		// Otherwise, add new handler to the end of the list
		for(handler = node->handler; NULL != handler->next; handler = handler->next);
		handler->next = new_handler;
	}
}

void dispatch_table_execute(const char* _target,
							const char* _verb,
							json_t* args)
{

	HC_ObjectNode* node = find_node(_target);
	if (NULL == node)
	{
		return;
	}

	HC_Handler* handler = node->handler;
	for(; NULL != handler; handler = handler->next)
	{
		if (0 == strcmp(handler->name, _verb))
		{
			if (handler->pre != NULL)
			{
				handler->pre(handler->preArgs);
			}
			
			handler->op(node->object, args);

			if (handler->post != NULL)
			{
				handler->post(handler->postArgs);	
			}
		}
	}
}