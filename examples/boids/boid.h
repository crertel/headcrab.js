#pragma once
#ifndef BOID_H
#define BOID_H

#include <stdlib.h>

#define MAP_WIDTH 300
#define MAP_HEIGHT 300

#define PI_CONSTANT 3.14159265358979
#define BOID_LENGTH 1.0
#define BOID_CRUISE_DIST BOID_LENGTH * 2.0
#define MAX_VELOCITY 5.0
#define BOID_CRUISE_SPEED MAX_VELOCITY / 5.0
#define MAX_ACCELERATION 5.0
#define MAX_TURN_RATE 5.0
#define BOID_VIEW_DIST 5.0
#define BOID_VIEW_ARC (270.0 / 360.0) * PI_CONSTANT

// Separation
// Alignment
// Cohesion

typedef struct Dimensions
{
	double length;
	double width;
} Dimensions;

typedef	struct Position
{
	double x;
	double y;
} Position;

typedef struct Velocity
{
	double speed;
	double dir;
} Velocity;

typedef struct Accel
{
	double accel;
	double dir;
} Accel;

typedef struct Boid
{
	int boidID;
	struct Dimensions size;
	struct Position pos;
	struct Velocity vel;
	struct Accel acc;
} Boid;

Boid * boid_random_init();

void boid_free(Boid * dead_boid);

#endif//BOID_H