#include "headcrab_dispatch.h"


/*
	Removes the node with the name _name from the table, including all handlers
	for that name.

	Pre: _name is not the name of the first object node in the table
*/
void dispatch_table_remove(HC_ObjectNode* _table, const char* _name)
{
	HC_ObjectNode *prev_node, *cur_node;
	prev_node = NULL;
	cur_node = _table;

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
HEADCRAB_ERROR dispatch_table_add(	HC_ObjectNode* _table,
									void* _target,
			                        const char* _name,
			                        const char* _verb,
			                        HC_PreOpFucntion _preOp,
			                        const void* _preOpArgs,
			                        HC_MutatorFunction _op,
			                        HC_PostOpFucntion _postOp,
			                        const void* _postOpArgs
									)
{
	HC_ObjectNode* node;
	HEADCRAB_ERROR ret;

	if (HC_SUCCESS != (ret = add_or_find_node(&node, _table, _target, _name)))
	{
		return ret;
	}
	node_add_handler(node, _verb, _preOp, _preOpArgs, _postOp, _postOpArgs, _op);
	return HC_SUCCESS;
}

/*
	Tries to find the object node in the table. If it is found, sets out to the
	pointer. If an object node already using that name is found, returns
	HC_FAIL. Otherwise, allocates a new node, adds it to the list, sets out to
	the pointer, and returns HC_SUCCESS.
*/
HEADCRAB_ERROR add_or_find_node(HC_ObjectNode** out,
								HC_ObjectNode* _table,
								void* _target,
								const char* _name
					 			)
{
	HC_ObjectNode *prev_node, *cur_node, *new_node;

	// Search for the target's entry
	prev_node = NULL;
	cur_node = _table;
	while(NULL != cur_node)
	{
		if (0 == strcmp(cur_node->name, _name))
		{
			if (cur_node->object != _target)
			{
				LOG_MSG("Could not add node: duplicate name")
				return HC_FAIL;
			}
			else
			{
				// We found a match!
				break;
			}
		}
		prev_node = cur_node;
		cur_node = cur_node->next;
	}

	if (NULL == cur_node)
	{
		new_node = malloc(sizeof(HC_ObjectNode));
		if (new_node == NULL)
		{
			LOG_ERROR("Unable to malloc HC_ObjectNode in add_or_find_node");
			exit(0);
		}

		new_node = {_name, NULL, NULL, _target};

		prev_node->next = new_node;

		cur_node = new_node;
	}

	*out = cur_node;
	return HC_SUCCESS;
}

/*
	Adds a new handler for the specified verb to the node.
*/
void node_add_handler(	HC_ObjectNode* node,
                        const char* _verb,
                        HC_PreOpFucntion _preOp,
                        const void* _preOpArgs,
                        HC_MutatorFunction _op,
                        HC_PostOpFucntion _postOp,
                        const void* _postOpArgs
                      )
{
	HC_Handler *handler, *new_handler;

	// Make the new handler
	new_handler = malloc(sizeof(HC_Handler));
	if (NULL == new_handler)
	{
		LOG_ERROR("Unable to malloc HC_Handler in node_add_handler");
		exit(0);
	}

	new_handler = {_verb, NULL, _preOp, _postOp, _preOpArgs, _postOpArgs, _op);

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