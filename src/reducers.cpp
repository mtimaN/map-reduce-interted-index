#include <pthread.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>

#include "reducers.h"

static bool comp_entries(std::pair<std::string, std::set<int>> &a,
                  std::pair<std::string, std::set<int>> &b) {
    if (a.second.size() == b.second.size()) {
        return a.first < b.first;
    }
    return a.second.size() > b.second.size();
}

void reduce(std::vector<std::map<std::string, std::set<int>>> &reduce_maps,
            pthread_mutex_t *reducers_tasks_mutex, std::queue<char> &char_queue) {
    while (true) {
        pthread_mutex_lock(reducers_tasks_mutex);
        if (char_queue.size() == 0) {
            pthread_mutex_unlock(reducers_tasks_mutex);
            break;
        }
        auto ch = char_queue.front();
        char_queue.pop();
        pthread_mutex_unlock(reducers_tasks_mutex);

        std::ofstream fout(std::string(1, ch) + ".txt");

        std::unordered_map<std::string, std::set<int>> char_map;
        std::vector<std::pair<std::string, std::set<int>>> sorted_entries;
        for (const auto &m : reduce_maps) {
            auto lower = m.lower_bound(std::string(1, ch));
            auto upper = m.lower_bound(std::string(1, ch + 1));
            for (auto it = lower; it != upper; ++it) {
                char_map[it->first].insert(it->second.begin(), it->second.end());
            }
        }

        for (const auto &p : char_map) {
            sorted_entries.push_back(p);
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
