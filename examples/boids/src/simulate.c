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

Boid * boid_list[BOID_COUNT];
static const int current_boid_count = BOID_COUNT;

/*
	Standard distance between two points.
	
	@param _pos_a - Point A.
	@param _pos_b - Point B.
	@return Distance between point A and B.
*/
double distance(Position _pos_a, Position _pos_b)
{
	return sqrt((_pos_a.x - _pos_b.x) * (_pos_a.x - _pos_b.x) +
				(_pos_a.y - _pos_b.y) * (_pos_a.y - _pos_b.y));
}

/*
	Calculates the angle between two boids,
	relative to the direction _dir.
	I believe this particular formula is defined for all inputs.

	@param _pos_a - Location of boid A.
	@param dir - Direction of boid A.
	@param _pos_b - Location of boid B.
	@return The relative angle from boid A to B, from -PI/2 to PI/2.
*/
double angle(Position _pos_a, double _dir, Position _pos_b)
{
	return atan((_pos_a.y - _pos_b.y) / (_pos_a.x - _pos_b.x)) - _dir;
}

/* 
	Confines _value between _lower_limit and _upper_limit.
	If _value exceeds either limit, _value is set to the respective limit.

	@param _value - The value to be bounded.
	@param _lower_limit - _value cannot be less than this.
	@param _upper_limit - _value cannot be greater than this.
	@return _value bound to those limits.
*/
double bound(double _value, double _lower_limit, double _upper_limit)
{
	if (_value < _lower_limit)
		_value = _lower_limit;
	if (_value > _upper_limit)
		_value = _upper_limit;
	return _value;
}

/*
	Determines which boids the specified _boid can see.

	@param[in] _boid - The reference boid.
	@param[in,out] _view_list - The list of boids that _boid can see.
	@return void.
*/
void fill_view_list(Boid * _boid, BoidView * _view_list)
{
	int i, j = 0;
	double dist, ang;
	Boid *other;
	for (i = 0; i < current_boid_count; ++i) {
		if (i == _boid->boidID)
			continue;
		other = boid_list[i];
		dist = distance(_boid->pos, other->pos);
		ang = angle(_boid->pos, _boid->vel.dir, other->pos);
		if (dist < BOID_VIEW_DIST && fabs(ang) < BOID_VIEW_ARC / 2) {
			_view_list[j] = (BoidView){other, dist, ang};
			j++;
		}
	}

	//printf("I found %d matches.\n", j);
	if (j < current_boid_count)
		_view_list[j] = (BoidView){NULL, 0.0, 0.0};
}

Accel attraction(Boid * _boid, BoidView * _other)
{
	Accel accel;
	accel.dir = _boid->vel.dir - _other->angle_to_boid;
	accel.accel = 0.1 * _other->dist_to_boid;
	accel.accel = bound(accel.accel, -MAX_ACCELERATION, MAX_ACCELERATION);
	return accel;
}

Accel total_influence(Boid * _boid)
{
	int i;
	BoidView view_list[current_boid_count];
	Accel attract;
	Accel update;

	fill_view_list(_boid, view_list);

	for(i = 0; view_list[i].boid != NULL; i++)
	{
		attract = attraction(_boid, &view_list[i]);
		update.accel += 0.1 * attract.accel;
		update.dir += 0.1 * attract.dir;
	}

	return update;
}

/* 
	Confines _value between _lower_limit and _upper_limit.
	If _value exceeds either limit, _value is wrapped around to the other side.

	@param _value - The value to be bounded.
	@param _lower_limit - _value cannot be less than this.
	@param _upper_limit - _value cannot be greater than this.
	@return _value wrapped to those limits.
*/
double wrap(double _value, double _lower_limit, double _upper_limit)
{
	double adjusted_value = 0;
	if (_upper_limit != 0)
		adjusted_value = _upper_limit * floor(_value / _upper_limit);
	_value -= adjusted_value + _lower_limit;

	return _value;
}

// dt = 1 does not change the accel
void update_boid(Boid *_boid, Accel _influence, double _dt)
{
	//_influence.accel /= _dt;

	// Should we set or influence accel?
	_boid->acc.accel += _influence.accel;
	_boid->acc.accel = bound(_boid->acc.accel, -MAX_ACCELERATION, MAX_ACCELERATION);
	_boid->acc.dir += _influence.dir;
	_boid->acc.dir = wrap(_boid->acc.dir, 0, 2 * PI_CONSTANT);
	// And then update velocity.
	_boid->vel.speed += _dt * _boid->acc.accel;
	_boid->vel.speed = bound(_boid->vel.speed, 0, MAX_VELOCITY);
	_boid->vel.dir += _dt * _boid->acc.dir;
	_boid->vel.dir = wrap(_boid->vel.dir, 0, 2 * PI_CONSTANT);
	// Update position.
	double dx = _boid->vel.speed / 
 				( sqrt(1 + tan(_boid->vel.dir) *
 			   			   tan(_boid->vel.dir)));
	_boid->pos.x += dx * _dt;
	_boid->pos.x = wrap(_boid->pos.y, 0, MAP_WIDTH);
	_boid->pos.y += dx * tan(_boid->vel.dir) * _dt;
	_boid->pos.y = wrap(_boid->pos.y, 0, MAP_HEIGHT);
}

static int running = 1;

/*
	Main update loop.
	Calculates the influence on each boid,
	then updates each boid accordingly.

	@param _time_step - The time that passes between each update.
	@return 0.
*/
int update(double _time_step)
{
	int i;
	Accel influence[current_boid_count];
	for (i = 0; i < current_boid_count; i++)
		influence[i] = total_influence(boid_list[i]);

	for (i = 0; i < current_boid_count; i++)
		update_boid(boid_list[i], influence[i], _time_step);

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
	int i;
	json_t *boids = json_array();

	for(i = 0; i < current_boid_count; i++)
	{
		json_array_append_new(boids, json_boid_data(boid_list[i]));
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

HEADCRAB_ERROR pause(void* _args) { running = 0; return HC_SUCCESS; }

HEADCRAB_ERROR unpause(void* _args) { running = 1; return HC_SUCCESS; }

void quit_simulation(void* _target, const json_t* _args)
{
	shutdownServer = 1;
}

int main(int argc, char ** argv)
{
	int i;
	const int initial_boid_count = current_boid_count;

	for (i = 0; i < initial_boid_count; ++i) {
		boid_list[i] = boid_random_init();
	}
	headcrab_init("");
	headcrab_bind_object(boid_list, "flock", "getall",
						 pause, NULL,
						 send_data_to_browser,
						 unpause, NULL);
	headcrab_bind_object(NULL, "flock", "pause",
	 					 pause, NULL,
	 					 NULL,
	 					 NULL, NULL);

	while(!shutdownServer)
	{
		if (running)
		{
			update(0.00001);
		}
		headcrab_handle_commands();
	}

	void headcrab_shutdown();

	return 0;
}