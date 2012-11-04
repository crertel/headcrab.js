#include "boid.h"

Boid * boid_random_init()
{
	static int unique_boidID = 1;
	Boid * new_boid = malloc(sizeof(Boid));
	if (!new_boid)
		return NULL;
	new_boid->boidID = unique_boidID++;
	new_boid->size = (Dimensions){ BOID_LENGTH, BOID_LENGTH * 0.5 };
	new_boid->pos = (Position){ (double)(rand() % MAP_WIDTH), 
								 (double)(rand() % MAP_HEIGHT) };
	new_boid->vel = (Velocity){ BOID_CRUISE_SPEED, 0.0 };
	new_boid->acc = (Accel){ 0.0, 0.0 };
	return new_boid;
}

void boid_free(Boid * dead_boid)
{
	free(dead_boid);
}