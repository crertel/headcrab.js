typedef struct
{
	char * name;
	ObjectNode * next;
	Handler * handler;
	void * object;
}

typedef struct
{
	char * name;
	Handler * next;
	PreOp * pre;
	PostOp * post;
	void * preArgs;
	void * postArgs;
	MutOp * op;
};