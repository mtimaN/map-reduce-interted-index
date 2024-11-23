#pragma once

#include <pthread.h>

#include <queue>
#include <string>
#include <set>
#include <map>


void reduce(std::queue<std::map<std::string, std::set<int>>> &reduce_maps,
	    pthread_mutex_t *reducers_tasks_mutex, pthread_barrier_t *reducers_barrier);
