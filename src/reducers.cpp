#include <pthread.h>

#include <iostream>
#include <fstream>
#include <algorithm>

#include "reducers.h"

bool comp_entries(std::pair<std::string, std::set<int>> &a,
                  std::pair<std::string, std::set<int>> &b) {
    if (a.second.size() == b.second.size()) {
        return a.first < b.first;
    }
    return a.second.size() > b.second.size();
}

static std::map<std::string, std::set<int>>
merge_maps(std::vector<std::map<std::string, std::set<int>>> &maps, int begin, int end) {
    std::map<std::string, std::set<int>> merged_map;
    for (int i = begin; i < end; ++i) {
        for (const auto &[key, value] : maps[i]) {
            merged_map[key].insert(value.begin(), value.end());
        }
    }
    return merged_map;
}

void reduce(unsigned int thread_id, unsigned int no_of_reducers,
            std::vector<std::map<std::string, std::set<int>>> &reduce_maps,
            std::map<std::string, std::set<int>> &final_map,
            pthread_mutex_t *reducers_tasks_mutex, pthread_barrier_t *reducers_barrier,
	    std::queue<std::pair<char, std::pair<std::map<std::string,
	    std::set<int>>::iterator, std::map<std::string, std::set<int>>::iterator>>>
	    &output_tasks) {

    int size = reduce_maps.size();
    int start = thread_id * (double)size / no_of_reducers;
    int end = std::min((int)((thread_id + 1) * (double)size / no_of_reducers), size);

    auto merged_map = merge_maps(reduce_maps, start, end);

    pthread_mutex_lock(reducers_tasks_mutex);
    for (const auto &[key, val] : merged_map) {
        final_map[key].insert(val.begin(), val.end());
    }
    pthread_mutex_unlock(reducers_tasks_mutex);

    pthread_barrier_wait(reducers_barrier);

    if (pthread_mutex_trylock(reducers_tasks_mutex) == 0) {
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

        std::vector<std::pair<std::string, std::set<int>>> sorted_entries;
        for (auto it = lower; it != upper; ++it) {
            sorted_entries.push_back(*it);
        }

        std::sort(sorted_entries.begin(), sorted_entries.end(), comp_entries);

        for (const auto &p : sorted_entries) {
            fout << p.first << ":[";
            for (const auto &val : p.second) {
                if (val != *p.second.begin()) {
                    fout << ' ';
                }

                fout << val;
            }
            fout << "]\n";
        }
        fout.close();
    }
}
