#pragma once

#include <pthread.h>

#include <queue>
#include <string>
#include <set>
#include <map>


/**
 * @brief receives a vector of partial maps and writes to disk as such:
 * <x>.txt - the complete list of all words starting with <x> and the ids of files
 * where such word occurs
 *
 * @param reduce_maps the list of partial maps
 * @param reducers_tasks_mutex mutex used for popping out of the queue
 * @param char_queue the given alphabet
 */
void reduce(std::vector<std::map<std::string, std::set<int>>> &reduce_maps,
	    pthread_mutex_t *reducers_tasks_mutex, std::queue<char> &char_queue);

