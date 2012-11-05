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

void calculate_view(Boid * _boid, BoidView ** view_list)
{
	int j = 0;
	double dist, ang;
	for (int i = 0; i < current_boid_count; ++i) {
		if (i == _boid->boidID)
			continue;
		printf("Calc view for %d %d", i, j);
		dist = distance(_boid->pos, boid_list[i]->pos);
		printf("Distance! %f\n", dist);
		ang = angle(_boid->pos, _boid->vel.dir, boid_list[i]->pos);
		printf("Angle! %f\n", ang);
		if (dist < BOID_VIEW_DIST && fabs(ang) < BOID_VIEW_ARC / 2) {
			(*view_list)[j].boid = boid_list[i];
			(*view_list)[j].dist_to_boid = dist;
			(*view_list)[j].angle_to_boid = ang;
			j++;
		}
	}
	printf("Boid is here: %p.\n",  (*view_list)[0].boid);
	printf("I found %d matches.\n", j);
	if (j < current_boid_count)
		(*view_list)[j].boid = NULL;
	printf("End of calc view! yay. \n");
}

// double calculate_turning(Boid * _boid)
// {

// }

// double calculate_repulsion(Boid * _boid)
// {

// }

void calculate_attraction(Boid * _boid, BoidView ** _view_list)
{
	int min_boid_id = 0;
	double min_dist = 0.0;
	double d_accel = 0.0;
	printf("Find closest boid.\n");
	for (int i = 0; i < current_boid_count && (*_view_list)[i].boid != NULL; ++i) {
		printf("Find %d\n", i);
		if ((*_view_list)[i].dist_to_boid > min_dist) {
			min_boid_id = i;
			min_dist = (*_view_list)[i].dist_to_boid;
		}
	}
	if ((*_view_list)[0].boid == NULL)
		return;
	_boid->vel.dir += 0.1 * (*_view_list)[min_boid_id].angle_to_boid;
	if (_boid->vel.dir > 2.0 * PI_CONSTANT)
		_boid->vel.dir -= 2.0 * PI_CONSTANT;
	else if (_boid->vel.dir < 0)
		_boid->vel.dir += 2.0 * PI_CONSTANT;
}

// double calculate_acceleration(Boid * _boid)
// {

// }

int update(double _time_step)
{
	BoidView view_list[20];
	double dx;
	for (int i = 0; i < current_boid_count; ++i) {
		calculate_view(boid_list[i], &view_list);
		calculate_attraction(boid_list[i], &view_list);
		dx = boid_list[i]->vel.speed / 
		 	( sqrt(1 + tan(boid_list[i]->vel.dir) *
		 			   tan(boid_list[i]->vel.dir))
			);
		boid_list[i]->pos.x += dx * _time_step;
		if (boid_list[i]->pos.x > MAP_WIDTH)
			boid_list[i]->pos.x -= MAP_WIDTH;
		if (boid_list[i]->pos.x < 0)
			boid_list[i]->pos.x += MAP_WIDTH;
		boid_list[i]->pos.y += dx * tan(boid_list[i]->vel.dir) * _time_step;
		if (boid_list[i]->pos.y > MAP_HEIGHT)
			boid_list[i]->pos.y -= MAP_HEIGHT;
		if (boid_list[i]->pos.y < 0)
			boid_list[i]->pos.y += MAP_HEIGHT;
	}
	return 0;
}

void send_data_to_browser(void* _target, const json_t* _args)
{
	json_t * message = json_object();
	json_object_set_new(message, "type", json_string("response"));
	json_t * data = json_object();
	json_t * boids = json_array();

	for(int i = 0; i < current_boid_count; ++i) {
		json_t * boid_data = json_object();
		json_object_set_new(boid_data, "x", json_real(boid_list[i]->pos.x));
		json_object_set_new(boid_data, "y", json_real(boid_list[i]->pos.y));
		json_object_set_new(boid_data, "theta", json_real(boid_list[i]->vel.dir));
		json_array_append_new(boids, boid_data);
	}
	json_object_set_new(data, "boids", boids);
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

	printf("Got to #0\n");
	for (int i = 0; i < initial_boid_count; ++i) {
		boid_list[i] = boid_random_init();
	}
	printf("Got to #1\n");
	headcrab_init("");
	headcrab_bind_object(boid_list, "flock", "getall",
						 NULL, NULL,
						 send_data_to_browser,
						 NULL, NULL);
	// headcrab_bind_object(NULL, "", "shutdown",
	// 					 NULL, NULL,
	// 					 quit_simulation,
	// 					 NULL, NULL);

	while(!shutdownServer)
	{
		update(0.001);
		headcrab_handle_commands();
	}

	void headcrab_shutdown();

	return 0;
}