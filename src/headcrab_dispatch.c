#include "headcrab_dispatch.h"

/*
	Adds the entry to the dispatch table.
*/
void dispatch_table_add(HC_ObjectNode* _table,
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
	add_or_find_node(&node, _table, _target, _name);
	node_add_handler(node, _verb, _preOp, _preOpArgs, _postOp, _postOpArgs, _op);
}

/*
	Tries to find the object node in the table. If it is found, sets returns the
	pointer via out. Otherwise, allocates a new node, adds it to the list,
	and returns a pointer to the node via out.
*/
void add_or_find_node(  HC_ObjectNode** out,
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
				// TODO log duplicate name error
				return;
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
			// TODO log fatal error
			exit(0);
		}

		new_node = {_name, NULL, NULL, _target};

		prev_node->next = new_node;

		cur_node = new_node;
	}

	*out = cur_node;
}

/*
	Attempts to add a handler to the node. If a handler with the same name,
	already exists, does nothing. Otherwise, adds the handler.
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
	HC_Handler *prev_handler, *cur_handler, *new_handler;

	// Make the new handler
	new_handler = malloc(sizeof(HC_Handler));
	if (NULL == new_handler)
	{
		// TODO log fatal error
		exit(0);
	}

	new_handler = {_verb, NULL, _preOp, _postOp, _preOpArgs, _postOpArgs, _op);

	// If this will be the first handler, add and exit.
	if (NULL == node->handler)
	{
		node->handler = new_handler;
	}
	else
	{
		// Search for a duplicate handler
		cur_handler = node->handler;
		prev_handler = NULL;
		while(NULL != cur_handler)
		{
			if (0 == strcmp(handler->name, handlerName))
			{
				// TODO log duplicate error
				return;
			}
			prev_handler = cur_handler;
			cur_handler = cur_handler->next;
		}
		cur_handler->next = new_handler;
	}
}