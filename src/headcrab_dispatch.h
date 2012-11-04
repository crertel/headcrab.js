#pragma once
#ifndef HEADER_DISPATCH_H
#define HEADER_DISPATCH_H

#include "headcrab.h"

typedef struct HC_Handler
{
	char * name;
	struct HC_Handler * next;
	HC_PreOpFunction pre;
	HC_PostOpFunction post;
	void * preArgs;
	void * postArgs;
	HC_MutatorFunction op;
} HC_Handler;

typedef struct HC_ObjectNode
{
	char * name;
	struct HC_ObjectNode * next;
	struct HC_Handler * handler;
	void * object;
} HC_ObjectNode;

void dispatch_table_init();

void free_node(HC_ObjectNode* _node);

void dispatch_table_remove(const char* _name);

void dispatch_table_rebind(const char* _name, void* _target);

HEADCRAB_ERROR dispatch_table_add(  void* _target,
			                        const char* _name,
			                        const char* _verb,
			                        HC_PreOpFunction _preOp,
			                        const void* _preOpArgs,
			                        HC_MutatorFunction _op,
			                        HC_PostOpFunction _postOp,
			                        const void* _postOpArgs
									);

HEADCRAB_ERROR add_or_find_node(HC_ObjectNode** out,
								void* _target,
								const char* _name
					 			);

void node_add_handler(	HC_ObjectNode* node,
                        const char* _verb,
                        HC_PreOpFunction _preOp,
                        const void* _preOpArgs,
                        HC_MutatorFunction _op,
                        HC_PostOpFunction _postOp,
                        const void* _postOpArgs
                      );

#endif//HEADER_DISPATCH_H