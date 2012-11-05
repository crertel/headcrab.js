#include "boid.h"
//#include "boid_list.h"
#include "headcrab.h"
#include <math.h>

typedef struct BoidView
{
	Boid * boid;
	double dist_to_boid;
	double angle_to_boid;
} BoidView;

Boid * boid_list[20];
static const int current_boid_count = 20;

double distance(Position a, Position b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) +
				(a.y - b.y) * (a.y - b.y));
}

double angle(Position a, double dir, Position b)
{
	return atan((a.y - b.y) / (a.x - b.x)) - dir;
}

void fill_view_list(Boid * _boid, BoidView * view_list)
{
	int j = 0;
	double dist, ang;
	Boid *other;
	for (int i = 0; i < current_boid_count; ++i) {
		if (i == _boid->boidID)
			continue;
		other = &(boid_list[i]);
		dist = distance(_boid->pos, other->pos);
		ang = angle(_boid->pos, _boid->vel.dir, other->pos);
		if (dist < BOID_VIEW_DIST && fabs(ang) < BOID_VIEW_ARC / 2) {
			view_list[j] = {other, dist, ang};
			j++;
		}
	}

	printf("I found %d matches.\n", j);
	if (j < current_boid_count)
		view_list[j] = {NULL, 0.0, 0.0};
}

Accel attraction(Boid * _boid, BoidView * _other)
{
	Accel accel;

	accel.dir = _other.angle_to_boid;
	accel.accel = _other.dist_to_boid;
}

Accel total_influence(Boid * _boid)
{
	int i;
	BoidView view_list[20];
	Accel attraction;
	Accel update;

	fill_view_list(_boid, view_list);

	for(i = 0; view_list[i].boid != NULL; i++)
	{
		Accel attraction = attraction(_boid, &view_list[i]);
	}

	update = attraction;

	return update;
}

// dt = 1 does not change the accel
void update(Boid *_boid, Accel _influence, double _dt=1)
{
	_influence.accel /= _dt;

	// Should we set or influence accel?

	// And then update velocity

	// And then update position

	/*
	dx = boid_list[i]->vel.speed / 
 	( sqrt(1 + tan(boid_list[i]->vel.dir) *
 			   tan(boid_list[i]->vel.dir))
	);
	if (boid_list[i]->pos.x > MAP_WIDTH)
		boid_list[i]->pos.x -= MAP_WIDTH;
	if (boid_list[i]->pos.x < 0)
		boid_list[i]->pos.x += MAP_WIDTH;
	boid_list[i]->pos.y += dx * tan(boid_list[i]->vel.dir) * _time_step;
	if (boid_list[i]->pos.y > MAP_HEIGHT)
		boid_list[i]->pos.y -= MAP_HEIGHT;
	if (boid_list[i]->pos.y < 0)
		boid_list[i]->pos.y += MAP_HEIGHT;*/
}


double wrap(double large, double limit)
{
	return large - (limit * floor(large / limit));
}

int update(double _time_step)
{
	int i;
	Accel influence[20];
	for (i = 0; i < current_boid_count; i++)
	{
		influence[i] = total_influence(&boid_list[i]);
	}

	for (i = 0; i < current_boid_count; i++)
	{
		update(&boid_list[i], influence[i], _time_step);
	}

	return 0;
}

json_t *json_boid_data(Boid *_boid)
{
	json_t * boid_data = json_object();
	json_object_set_new(boid_data, "x", json_real(_boid->pos.x));
	json_object_set_new(boid_data, "y", json_real(_boid->pos.y));
	json_object_set_new(boid_data, "theta", json_real(_boid->vel.dir));
	return boid_data;
}

json_t *json_boid_array()
{
	json_t *boids = json_array();

	for(int i = 0; i < current_boid_count; i++)
	{
		json_array_append_new(boids, json_boid_data(&boid_list[i]));
	}

	return boids;
}

void send_data_to_browser(void* _target, const json_t* _args)
{
	json_t * message = json_object();
	json_object_set_new(message, "type", json_string("response"));
	json_t * data = json_object();

	json_object_set_new(data, "boids", json_boid_array());
	json_object_set_new(message, "data", data);

	json_t * seq_id = json_object_get(_args, "seqID");
	int seqID = json_integer_value(seq_id);

	headcrab_post_message(seqID, message);
}

static int shutdownServer = 0;
static int running = 1;

HEADCRAB_ERROR pause(void* _args) { running = 0; }

HEADCRAB_ERROR unpause(void* _args) { running = 1; }

void quit_simulation(void* _target, const json_t* _args)
{
	shutdownServer = 1;
}

int main(int argc, char ** argv)
{
	const int initial_boid_count = current_boid_count;

	printf("Got to #0\n");
	for (int i = 0; i < initial_boid_count; ++i) {
		boid_list[i] = boid_random_init();
	}
	printf("Got to #1\n");
	headcrab_init("");
	headcrab_bind_object(boid_list, "flock", "getall",
						 pause, NULL,
						 send_data_to_browser,
						 unpause, NULL);
	// headcrab_bind_object(NULL, "", "shutdown",
	// 					 NULL, NULL,
	// 					 quit_simulation,
	// 					 NULL, NULL);

	while(!shutdownServer)
	{
		if (running)
		{
			update(0.001);			
		}
		headcrab_handle_commands();
	}

	void headcrab_shutdown();

	return 0;
}