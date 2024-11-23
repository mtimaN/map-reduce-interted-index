#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <string>
#include <queue>
#include <map>
#include <set>

std::map<std::string, std::set<int>>
map(std::queue<std::pair<std::string, int>> &file_names, pthread_mutex_t *mutex);
