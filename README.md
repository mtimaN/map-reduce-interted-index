## Tema1a

### Author: Matei Mantu 332CA

## Parallel Computations of an Inverted Index using Map-Reduce

### Short description

The program parses text from a given list of files and outputs a reverted index for each word found and the ids of the files where that word is found. The output is written in files named as <ch>.txt where <ch> is the starting character of the words found inside the file.

### Implementation details

The main function parses the list of file names and adds each file in a queue together with their id. It then creates a queue with the alphabet's characters. The threads are then created, each thread being designated as either a mapper or a reducer based on id.

#### Mappers

Mappers take tasks from the queue of file names and parse each word. They remove any letter not part of the english alphabet using `isalpha()` and turns all letters to lowercase.

In the end each mapper thread will return a "partial" map of type `std::map<std::string, std::set<int>>` - this way the entries are sorted by first letter initially and the indices are stored in increasing order. The "partial" map is inserted into a vector used by the reducers.

#### Reducers

The reducers receive a vector of maps from the mapper threads and a queue of the alphabet. Each letter <ch> represents a "task": iterate through the maps aggregate the data in order to create `<ch>.txt`. This approach is faster than merging all maps into one before writing to disk and is valid according to the forums.
