#include <stdlib.h>
#include "headcrab.h"
#include "headcrab_message_queue.h"
#include "queue.h"

#define MQ_LEN 100

static void* buffer_in[MQ_LEN];
static void* buffer_out[MQ_LEN];
static queue_t in = QUEUE_INITIALIZER(buffer_in);
static queue_t out = QUEUE_INITIALIZER(buffer_out);

void mq_push(MQ queue, void* value)
{
	if (MQ_IN == queue)
		queue_enqueue(&in, value);
	else if (MQ_OUT == queue)
		queue_enqueue(&out, value);
	else
		LOG_ERROR("Invalid queue push query!\n");
	return;
}

void* mq_pop(MQ queue)
{
	if (MQ_IN == queue)
		return queue_dequeue(&in);
	else if (MQ_OUT == queue)
		return queue_dequeue(&out);
	else {
		LOG_ERROR("Invalid queue pop query!\n");
		return NULL;
	}
}