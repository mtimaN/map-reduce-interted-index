# Compiler and flags
CXX := g++
CXXFLAGS := -g -lpthread -Wall -Werror

# Source files and output
SRCS := main.cpp mappers.cpp reducers.cpp
OBJS := $(SRCS:.cpp=.o)
TARGET := tema1

# Default rule
all: build

# Rule to build the target
build: $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(CXXFLAGS) $(TSAN_OPTIONS)

# Rule to build object files
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
