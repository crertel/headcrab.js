#pragma once
#ifndef BOID_LIST_H
#define BOID_LIST_H

#include "boid.h"

typedef struct BoidList
{
	Boid * boid;
	struct BoidList * next;
} BoidList;

void boidlist_init(BoidList ** _boid_list);

void boidlist_add(BoidList ** _boid_list, Boid * _boid);

void boidlist_remove(BoidList * _boid_list, Boid * _boid);

BoidList * boidlist_get_last_node();

#endif//BOID_LIST_H