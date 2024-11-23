#include "reducers.h"
#include <pthread.h>

#include <iostream>

static std::map<std::string, std::set<int>>
merge_maps(std::map<std::string, std::set<int>> &map1, std::map<std::string, std::set<int>> &map2) {
    for (const auto &p : map1) {
        if (map2.find(p.first) != map2.end()) {
            map1[p.first].insert(map2[p.first].begin(), map2[p.first].end());
            map2.erase(p.first);
        }
    }

    map1.insert(map2.begin(), map2.end());
    return map1;
}

void reduce(std::queue<std::map<std::string, std::set<int>>> &reduce_maps,
            pthread_mutex_t *reducers_tasks_mutex, pthread_barrier_t *reducers_barrier) {
    while (reduce_maps.size() > 1) {
        pthread_mutex_lock(reducers_tasks_mutex);
        if (reduce_maps.size() <= 1) {
            pthread_mutex_unlock(reducers_tasks_mutex);
            continue;
        }

        auto map1 = reduce_maps.front();
        reduce_maps.pop();
        auto map2 = reduce_maps.front();
        reduce_maps.pop();
        pthread_mutex_unlock(reducers_tasks_mutex);

        auto merged_map = merge_maps(map1, map2);

        pthread_mutex_lock(reducers_tasks_mutex);
        reduce_maps.push(merged_map);
        pthread_mutex_unlock(reducers_tasks_mutex);
    }

    pthread_barrier_wait(reducers_barrier);

    // TODO: split the map using std::upper_bound and std::lower_bound
    // and make a bag of tasks for writing
    pthread_mutex_lock(reducers_tasks_mutex);
}
