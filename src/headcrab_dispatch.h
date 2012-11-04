typedef struct
{
	char * name;
	ObjectNode * next;
	HC_Handler * handler;
	void * object;
} HC_ObjectNode;

typedef struct
{
	char * name;
	HC_Handler * next;
	HC_PreOpFunction * pre;
	HC_PostOpFunction * post;
	void * preArgs;
	void * postArgs;
	HC_MutatorFunction * op;
} HC_Handler;

void dispatch_table_remove(HC_ObjectNode* _table, const char* _name);

HEADCRAB_ERROR dispatch_table_add(	HC_ObjectNode* _table,
									void* _target,
			                        const char* _name,
			                        const char* _verb,
			                        HC_PreOpFucntion _preOp,
			                        const void* _preOpArgs,
			                        HC_MutatorFunction _op,
			                        HC_PostOpFucntion _postOp,
			                        const void* _postOpArgs
									);

HEADCRAB_ERROR add_or_find_node(HC_ObjectNode** out,
								HC_ObjectNode* _table,
								void* _target,
								const char* _name
					 			);

void node_add_handler(	HC_ObjectNode* node,
                        const char* _verb,
                        HC_PreOpFucntion _preOp,
                        const void* _preOpArgs,
                        HC_MutatorFunction _op,
                        HC_PostOpFucntion _postOp,
                        const void* _postOpArgs
                      );