# Dining Philosophers Problem Implementation

## Overview

This project implements the classic Dining Philosophers problem in both C and C++. The problem illustrates challenges in resource allocation and deadlock prevention in concurrent programming. The implementation uses POSIX threads and synchronization primitives to manage concurrent access to shared resources.

## Problem Description

Five philosophers sit around a circular table, alternating between thinking and eating. Between each pair of philosophers is a single fork. To eat, a philosopher needs both the fork to their left and to their right. The challenge is to design a solution that avoids deadlock and ensures fair access to resources.

## Features

- Two implementations: C and C++ versions
- Concurrent execution using POSIX threads
- Deadlock prevention using mutex and condition variables
- Real-time state visualization
- Thread-safe output synchronization
- Modern C++ features and RAII principles
- Exception-based error handling

## Requirements

- Unix-like operating system (Linux, macOS)
- GCC compiler (for C version)
- G++ compiler (for C++ version)
- POSIX threads support
- C++11 or later

## Building the Programs

You can build both programs using the provided Makefile:

```bash
make
```

This will create two executables:

- `philosopher` (C version)
- `diningPhilosopher` (C++ version)

## Running the Programs

You can run either version:

```bash
# For C version
./philosopher

# For C++ version
./diningPhilosopher
```

## Output Legend

Both programs display the state of each philosopher in real-time:

- T: Thinking
- H: Hungry
- E: Eating

Example output:

```
-----T represents THINKING, H represents HUNGRY, E represents EATING-----
philosopher 1 is HUNGRY      T H T T T
philosopher 1 is EATING      T E T T T
philosopher 2 is HUNGRY      T E H T T
```

## Implementation Details

### C Version

- Uses semaphores for resource management
- Implements a mutex for synchronized output
- Handles SIGINT signal for clean program termination
- Uses thread-safe memory management

### C++ Version

- Uses mutex and condition variables for synchronization
- Object-oriented design with `DiningPhilosophers` class
- RAII for resource management
- Exception-based error handling
- Type-safe state management using enum class
- Thread-safe data structures using std::array
- Modern C++ features (constexpr, range-based for loops, etc.)

## Code Structure

### C Version

- `philosopher.c`: Main implementation file
- Key functions:
  - `philosopher()`: Main philosopher behavior
  - `take_forks()`: Resource acquisition
  - `put_forks()`: Resource release
  - `test()`: Check if philosopher can eat
  - `show_state()`: Display system state

### C++ Version

- `dining_philosopher.cpp`: Main implementation file
- `DiningPhilosophers` class:
  - Constructor/Destructor: RAII for resource management
  - `run()`: Main program execution
  - `takeForks()`: Resource acquisition
  - `putForks()`: Resource release
  - `test()`: Check if philosopher can eat
  - `showState()`: Display system state
  - `PhilosopherData` struct: Thread data management
  - Private helper methods for thread management

## Clean Up

To remove compiled files:

```bash
make clean
```

## Common Issues

1. If you see compilation warnings:

   - Make sure you have the latest version of the code
   - All warnings have been addressed in the current version

2. If output appears garbled:

   - This is likely due to terminal buffering
   - Try running with `stdbuf -o0 ./philosopher` or `stdbuf -o0 ./diningPhilosopher`

3. If you encounter runtime errors:
   - Check if you have proper permissions
   - Ensure all required libraries are installed
   - The program will display specific error messages for common issues
