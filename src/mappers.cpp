#include <pthread.h>

#include <queue>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

#include "mappers.h"

using std::string, std::pair, std::set, std::queue;

/**
 * @brief removes unwanted character from word and converts to lowercase
 *
 * @param word 
 * @return cleaned word
 */
static string clean_word(const string& word) {

    string cleaned = word;
    cleaned.erase(std::remove_if(cleaned.begin(), cleaned.end(), [](char c) {
        return !isalpha(c);
    }), cleaned.end());

    std::transform(cleaned.begin(), cleaned.end(), cleaned.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return cleaned;
}


/**
 * @brief parse words from given file
 *
 * @param reverted_indices 
 * @param file_info 
 */
static void parse_words(std::map<string, set<int>> &reverted_indices,
                 pair<string, int> file_info) {
    std::ifstream file(file_info.first);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file.\n";
        return;
    }

    string word;

    while (file >> word) {
        word = clean_word(word);
        reverted_indices[word].insert(file_info.second);
    }
}

std::map<string, set<int>>
map(queue<pair<string, int>> &file_names, pthread_mutex_t *mutex) {
    std::map<string, set<int>> reverted_indices;
    while (true) {
        pthread_mutex_lock(mutex);
        if (file_names.size() == 0) {
            pthread_mutex_unlock(mutex);
            break;
        }
        auto file_info = file_names.front();
        file_names.pop();
        pthread_mutex_unlock(mutex);

        parse_words(reverted_indices, file_info);
    }

    return reverted_indices;
}
