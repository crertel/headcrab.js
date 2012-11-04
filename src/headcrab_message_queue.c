typedef struct
{
	HC_QNode* next;
	void* value;
} HC_QNode;

typedef struct
{
	HC_QNode* head;
	HC_QNode* tail;
} HC_Queue;

static HC_Queue queues[2];

void _mq_push(HC_Queue* q, void* value)
{
	HC_QNode* new_node = malloc(sizeof(HC_QNode));
	if (NULL == new_node)
	{
		LOG_ERROR("Could not allocate HC_QNode in mq_push\n");
		exit(0);
	}

	*new_node = (HC_QNode){NULL, value};

	if(NULL == q->tail)
	{
		q->head = q->tail = new_node;
	}
	else
	{
		q->tail->next = new_node;
		q->tail = new_node;
	}
}

void* _mq_pop(HC_Queue* q)
{
	if (NULL == q->head)
	{
		return NULL;
	}

	HC_QNode* node = q->head;
	void* data = node->value;

	if (q->head == q->tail)
	{
		q->head = q->tail = NULL;
	}
	else
	{
		q->head = node->next;
	}

	free(node);
	return data;
}

void mq_push(enum MQ queue, void* value)
{
	_mq_push(queues[queue], value);
}

void* mq_pop(enum MQ queue)
{
	return _mq_pop(queues[queue]);
}