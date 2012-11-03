typedef struct
{
	char * name;
	ObjectNode * next;
	HC_Handler * handler;
	void * object;
}

typedef struct
{
	char * name;
	HC_Handler * next;
	HC_PreOpFunction * pre;
	HC_PostOpFunction * post;
	void * preArgs;
	void * postArgs;
	HC_MutatorFunction * op;
};