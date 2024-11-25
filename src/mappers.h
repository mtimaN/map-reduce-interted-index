#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <string>
#include <queue>
#include <map>
#include <set>

/**
 * @brief receives a list of files and ids, each thread creates a partial map
 * of the words and the ids where each word occured
 *
 * @param file_names list of files to be read and their ids
 * @param mutex used for atomically popping the queue
 */
std::map<std::string, std::set<int>>
map(std::queue<std::pair<std::string, int>> &file_names, pthread_mutex_t *mutex);
