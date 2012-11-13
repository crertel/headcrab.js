#pragma once
#ifndef HEADCRAB_MQUEUE_H
#define HEADCRAB_MQUEUE_H

typedef enum MQ {
	MQ_IN,
	MQ_OUT
} MQ;

void mq_push(MQ queue, void* value);
void* mq_pop(MQ queue);

#endif//HEADCRAB_MQUEUE_H