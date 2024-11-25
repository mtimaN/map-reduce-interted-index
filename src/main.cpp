#include <stdlib.h>
#include <pthread.h>

#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <set>

#include "mappers.h"
#include "reducers.h"

typedef struct thread_function_args {
    unsigned int thread_id;
    unsigned int no_of_mappers;
    unsigned int no_of_reducers;
    std::queue<std::pair<std::string, int>> *file_queue;
    pthread_mutex_t *file_names_mutex;
    pthread_mutex_t *reducers_tasks_mutex;
    pthread_barrier_t *map_reduce_barrier;

    // shared variables empty at first
    std::vector<std::map<std::string, std::set<int>>> *reduce_maps;
    std::queue<char> *char_queue;
} thread_function_args_t;

void *thread_function(void *args) {
    thread_function_args_t *typed_args = (thread_function_args_t*) args;

    if (typed_args->thread_id < typed_args->no_of_mappers) {
        // mapper
        auto partial_map = map(*typed_args->file_queue, typed_args->file_names_mutex);

        pthread_mutex_lock(typed_args->reducers_tasks_mutex);
        typed_args->reduce_maps->push_back(partial_map);
        pthread_mutex_unlock(typed_args->reducers_tasks_mutex);

        pthread_barrier_wait(typed_args->map_reduce_barrier);
    } else {
        // reducer
        pthread_barrier_wait(typed_args->map_reduce_barrier);
        reduce(*typed_args->reduce_maps,
               typed_args->reducers_tasks_mutex,
               *typed_args->char_queue);
    }

    return NULL;
}

std::queue<std::pair<std::string, int>> parse_input(std::string input_file) {
    std::ifstream fin(input_file);
    int no_of_entries;
    std::string line;

    getline(fin, line);
    no_of_entries = stoi(line);

    std::queue<std::pair<std::string, int>> file_queue;
    for (int i = 1; i <= no_of_entries; ++i) {
        getline(fin, line);
        file_queue.push({line, i});
    }

    fin.close();

    return file_queue;
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        std::cout << "Input format: ./tema1 <number_of_mappers> <number_of_reducers> <input_file>";
        return 0;
    }

    unsigned int mappers = atoi(argv[1]);
    unsigned int reducers = atoi(argv[2]);
    thread_function_args_t *args = new thread_function_args_t[mappers + reducers];
    pthread_t *threads = new pthread_t[mappers + reducers];

    pthread_barrier_t map_reduce_barrier;
    pthread_barrier_init(&map_reduce_barrier, NULL, mappers + reducers);

    pthread_mutex_t file_names_mutex;
    pthread_mutex_init(&file_names_mutex, NULL);

    pthread_mutex_t reducers_tasks_mutex;
    pthread_mutex_init(&reducers_tasks_mutex, NULL);

    auto file_info = parse_input(argv[3]);
    if (file_info.size() == 0) {
        return 0;
    }

    std::vector<std::map<std::string, std::set<int>>> reduce_maps;
    std::queue<char> char_queue;
    for (char i = 'a'; i <= 'z'; ++i) {
        char_queue.push(i);
    }

    for (unsigned int i = 0; i < mappers + reducers; ++i) {
        args[i] = thread_function_args_t{i, mappers, reducers, &file_info, &file_names_mutex,
            &reducers_tasks_mutex, &map_reduce_barrier, &reduce_maps, &char_queue};
        int r = pthread_create(&threads[i], NULL, thread_function, &args[i]);

        if (r) {
            perror("pthread_create");
            exit(-1);
        }
    }

    for (unsigned int i = 0; i < mappers + reducers; ++i) {
        void *status;
        int r = pthread_join(threads[i], &status);

        if (r) {
            perror("pthread_join");
            exit(-1);
        }
    }

    pthread_barrier_destroy(&map_reduce_barrier);
    pthread_mutex_destroy(&file_names_mutex);
    pthread_mutex_destroy(&reducers_tasks_mutex);
    delete[](args);
    delete[](threads);
    return 0;
}
