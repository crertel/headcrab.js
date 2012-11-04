#include "boid.h"
#include "boid_list.h"
#include "headcrab.h"
#include <math.h>

BoidList * boid_list = NULL;
static const int current_boid_count = 20;

// double distance(Position a, Position b)
// {
// 	return sqrt(a.x * a.x + b.x * b.x);
// }

// double abs_angle(Position a, double dir, Position b)
// {

// }

// double calculate_view(Boid * _boid, BoidList * _view_list)
// {
// 	for (int i = 0; i < current_boid_count; ++i) {
// 		if (distance(boid->pos, boid_list[i].boid->pos) < BOID_VIEW_DIST &&
// 			abs_angle(boid->pos, boid->vel.dir, boid_list[i].boid->pos) < BOID_VIEW_ARC / 2)
// 			boidlist_add(_view_list, &boid_list[i]);
// 	}
// }

// double calculate_turning(Boid * _boid)
// {

// }

// double calculate_repulsion(Boid * _boid)
// {

// }

// double calculate_attraction(Boid * _boid)
// {

// }

// double calculate_acceleration(Boid * _boid)
// {

// }

int update(double _time_step)
{
	double dx = 0.0;
	for (int i = 0; i < current_boid_count; ++i) {
		dx = boid_list[i].boid->vel.speed / 
					( sqrt(1 + tan(boid_list[i].boid->vel.dir) * tan(boid_list[i].boid->vel.dir)) );
		boid_list[i].boid->pos.x += dx * _time_step;
		if (boid_list[i].boid->pos.x > MAP_WIDTH)
			boid_list[i].boid->pos.x -= MAP_WIDTH;
		boid_list[i].boid->pos.y += dx * tan(boid_list[i].boid->vel.dir) * _time_step;
		if (boid_list[i].boid->pos.y > MAP_HEIGHT)
			boid_list[i].boid->pos.y -= MAP_HEIGHT;
	}
	return 0;
}

void send_data_to_browser(void* _target, const json_t* _args)
{
	json_t * message = json_object();
	json_object_set_new(message, "type", json_string("response"));
	json_t * data = json_array();

	for(int i = 0; i < current_boid_count; ++i) {
		json_t * boid_data = json_object();
		json_object_set_new(boid_data, "x", json_real(boid_list[i].boid->pos.x));
		json_object_set_new(boid_data, "y", json_real(boid_list[i].boid->pos.y));
		json_object_set_new(boid_data, "theta", json_real(boid_list[i].boid->vel.dir));
		json_array_append_new(data, boid_data);
	}
	json_object_set_new(message, "data", data);

	json_t * seq_id = json_object_get(_args, "seqID");
	int seqID = json_integer_value(seq_id);

	headcrab_post_message(seqID, message);
}

static int shutdownServer = 0;

void quit_simulation(void* _target, const json_t* _args)
{
	shutdownServer = 1;
}

int main(int argc, char ** argv)
{
	const int initial_boid_count = current_boid_count;
	boidlist_init(boid_list);
	for (int i = 0; i < initial_boid_count; ++i)
		boidlist_add(boid_list, boid_random_init());

	headcrab_init("");
	headcrab_bind_object(boid_list, "flock", "getall",
						 NULL, NULL,
						 send_data_to_browser,
						 NULL, NULL);
	headcrab_bind_object(boid_list, "", "shutdown",
						 NULL, NULL,
						 quit_simulation,
						 NULL, NULL);

	while(!shutdownServer)
	{
		update(0.1);
		headcrab_handle_commands();
	}

	void headcrab_shutdown();

	return 0;
}