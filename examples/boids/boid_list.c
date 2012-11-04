#include "boid_list.h"

void boidlist_init(BoidList * _boid_list)
{
	_boid_list = malloc(sizeof(BoidList));
	_boid_list->boid = NULL;
	_boid_list->next = NULL;
}

void boidlist_add(BoidList * _boid_list, Boid * _boid)
{
	if (_boid_list == NULL) {
		boidlist_init(_boid_list);
		_boid_list[0].boid = _boid;
		return;
	}
	BoidList * last_node = boidlist_get_last_node(_boid_list);
	BoidList * new_node = malloc(sizeof(BoidList));
	last_node->next = new_node;
	new_node->boid = _boid;
	new_node->next = NULL;
}

void boidlist_remove(BoidList * _boid_list, Boid * _boid)
{
	if (_boid_list == NULL)
		return;
	BoidList * last = _boid_list;
	for (; last->next != NULL; last = last->next) {
		if (last->next->boid->boidID == _boid->boidID) {
			boid_free(last->next->boid);
			last->next = last->next->next;
			free(last->next);
			return;
		}
	}
	if (last == _boid_list)
		_boid_list = NULL;
}

int boidlist_size(BoidList * _boid_list)
{
	if (_boid_list == NULL)
		return 0;
	BoidList * last = _boid_list;
	int i = 0;
	while(last->next != NULL) {
		last = last->next;
		i++;
	}
	return i;
}

static BoidList * boidlist_get_last_node(BoidList * _boid_list)
{
	if (_boid_list == NULL)
		return NULL;
	BoidList * last = _boid_list;
	for (; last->next != NULL; last = last->next);
	return last;
}