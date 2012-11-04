#pragma once
#ifndef HEADCRAB_MESSAGE_QUEUE_H
#define HEADCRAB_MESSAGE_QUEUE_H

typedef enum MQ {
	MQ_IN,
	MQ_OUT
} MQ;

void mq_push(MQ queue, void* value);
void* mq_pop(MQ queue);

#endif//HEADCRAB_MESSAGE_QUEUE_H