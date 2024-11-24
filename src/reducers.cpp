#include <pthread.h>

#include <iostream>
#include <fstream>

#include "reducers.h"

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
            pthread_mutex_t *reducers_tasks_mutex, pthread_barrier_t *reducers_barrier,
	    std::queue<std::pair<char, std::pair<std::map<std::string,
	    std::set<int>>::iterator, std::map<std::string, std::set<int>>::iterator>>>
	    &output_tasks) {

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

    std::map<std::string, std::set<int>> final_map;

    if (pthread_mutex_trylock(reducers_tasks_mutex) == 0) {
        final_map = reduce_maps.front();
        for (char i = 'a'; i <= 'z'; ++i) {
            auto letter = std::string(1, i);
            auto lower = final_map.lower_bound(letter);
            auto upper = final_map.lower_bound(std::string(1, i + 1));
            output_tasks.push({i, {lower, upper}});
        }
        pthread_mutex_unlock(reducers_tasks_mutex);
    }

    pthread_barrier_wait(reducers_barrier);

    while (true) {
        pthread_mutex_lock(reducers_tasks_mutex);
        if (output_tasks.size() == 0) {
            pthread_mutex_unlock(reducers_tasks_mutex);
            break;
        }
        auto task = output_tasks.front();
        output_tasks.pop();
        pthread_mutex_unlock(reducers_tasks_mutex);

        std::ofstream fout(std::string(1, task.first) + ".txt");
        auto lower = task.second.first;
        auto upper = task.second.second;
        for (auto it = lower; it != upper; ++it) {
            fout << it->first << ":[";
            for (auto val_it = it->second.begin(); val_it != it->second.end(); ++val_it) {
                if (val_it != it->second.begin()) {
                    fout << ' ';
                }
                fout << *val_it;
            }
            fout << "]\n";
        }
        fout.close();
    }
}
