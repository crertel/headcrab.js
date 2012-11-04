enum MQ {
	MQ_IN,
	MQ_OUT
};

void mq_push(HC_Queue* q, void* value);
void* mq_pop(HC_Queue* q);