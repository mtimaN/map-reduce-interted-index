#pragma once

#include <pthread.h>

#include <queue>
#include <string>
#include <set>
#include <map>


void reduce(unsigned int id, unsigned int number_of_reducers,
	    std::vector<std::map<std::string, std::set<int>>> &reduce_maps,
	    std::map<std::string, std::set<int>> &final_map,
	    pthread_mutex_t *reducers_tasks_mutex, pthread_barrier_t *reducers_barrier,
	    std::queue<std::pair<char, std::pair<std::map<std::string,
	    std::set<int>>::iterator, std::map<std::string, std::set<int>>::iterator>>>
	    &output_tasks);
