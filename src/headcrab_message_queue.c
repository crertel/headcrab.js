#include <stdlib.h>
#include "headcrab.h"
#include "headcrab_message_queue.h"
#include "queue.h"

#define MQ_LEN 100

static void* buffer[MQ_LEN];
static queue_t in = QUEUE_INITIALIZER(buffer);

void mq_push(MQ queue, void* value)
{
	if (MQ_IN == queue)
	{
		queue_enqueue(&in, value);
	}
	
}

void* mq_pop(MQ queue)
{
	if (MQ_IN == queue)
	{
		return queue_dequeue(&in);
	}
	else
	{
		return NULL;
	}
}