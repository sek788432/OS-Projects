# Compilers
CC = gcc
CXX = g++

# Compiler flags
CFLAGS = -Wall -pthread
CXXFLAGS = -Wall -pthread -std=c++11

# Targets
TARGETS = philosopher diningPhilosopher

# Source files
C_SRCS = philosopher.c
CPP_SRCS = dining_philosopher.cpp

# Object files
C_OBJS = $(C_SRCS:.c=.o)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)

# Default target
all: $(TARGETS)

# C program
philosopher: $(C_OBJS)
	$(CC) $(C_OBJS) -o $@ $(CFLAGS)

# C++ program
diningPhilosopher: $(CPP_OBJS)
	$(CXX) $(CPP_OBJS) -o $@ $(CXXFLAGS)

# C object files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# C++ object files
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Clean target
clean:
	rm -f $(C_OBJS) $(CPP_OBJS) $(TARGETS)

.PHONY: all clean 