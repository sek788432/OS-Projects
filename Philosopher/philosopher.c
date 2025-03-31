#include <fcntl.h> /* For O_* constants */
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>  // Add this with other includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>

#define N 5
#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N

#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[N];
volatile sig_atomic_t running = 1;
pthread_t thread[N];
sem_t *waittype;
sem_t *posttype[N];
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// Add timeout structure
struct timespec timeout = {.tv_sec = 1,  // 1 second timeout
                           .tv_nsec = 0};

void show_state() {
    pthread_mutex_lock(&print_mutex);
    printf(" | ");
    for (int t = 0; t < N; t++) {
        if (state[t] == THINKING)
            printf("T");
        else if (state[t] == HUNGRY)
            printf("H");
        else if (state[t] == EATING)
            printf("E");
    }
    printf("\n");
    fflush(stdout);
    pthread_mutex_unlock(&print_mutex);
}

void test(int i) {
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        pthread_mutex_lock(&print_mutex);
        printf("Philosopher %d: EATING   ", i + 1);
        pthread_mutex_unlock(&print_mutex);
        show_state();
        sem_post(posttype[i]);  // up(), increase semaphore
    }
}

void take_forks(int i) {
    while (running) {
        if (sem_wait(waittype) == 0) {
            if (!running) {
                sem_post(waittype);
                return;
            }
            state[i] = HUNGRY;
            pthread_mutex_lock(&print_mutex);
            printf("Philosopher %d: HUNGRY   ", i + 1);
            pthread_mutex_unlock(&print_mutex);
            show_state();
            test(i);
            sem_post(waittype);

            while (running) {
                if (sem_trywait(posttype[i]) == 0) {
                    return;  // Got the forks
                }
                if (!running) return;
                usleep(100000);  // Sleep for 100ms
            }
        }
        if (!running) return;
    }
}

void put_forks(int i) {
    sem_wait(waittype);
    state[i] = THINKING;
    pthread_mutex_lock(&print_mutex);
    printf("Philosopher %d: THINKING ", i + 1);
    pthread_mutex_unlock(&print_mutex);
    show_state();
    test(LEFT);
    test(RIGHT);
    sem_post(waittype);
}

// Add signal handler function
void sigint_handler(int signum) {
    printf("\nReceived signal %d (Ctrl+C). Cleaning up...\n", signum);
    running = 0;

    // Wake up any waiting philosophers
    for (int i = 0; i < N; i++) {
        sem_post(posttype[i]);
    }
}

// Modify philosopher function
void *philosopher(void *arg) {
    int i = *((int *)arg);
    free(arg);  // Free the allocated memory

    while (running) {
        pthread_mutex_lock(&print_mutex);
        printf("Philosopher %d: THINKING ", i + 1);
        pthread_mutex_unlock(&print_mutex);
        show_state();
        sleep(1);

        if (!running) break;
        take_forks(i);

        if (!running) {
            sem_wait(waittype);
            state[i] = THINKING;
            test(LEFT);
            test(RIGHT);
            sem_post(waittype);
            break;
        }

        sleep(2);
        put_forks(i);
    }
    pthread_mutex_lock(&print_mutex);
    printf("Philosopher %d is exiting\n", i + 1);
    pthread_mutex_unlock(&print_mutex);
    return NULL;
}

// Modify main function
int main(void) {
    // Set up signal handler first
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    // Initialize named semaphores
    waittype = sem_open("/waittype", O_CREAT, 0644, 1);
    if (waittype == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    for (int t = 0; t < N; t++) {
        char sem_name[20];
        snprintf(sem_name, sizeof(sem_name), "/posttype%d", t);
        posttype[t] = sem_open(sem_name, O_CREAT, 0644, 0);
        if (posttype[t] == SEM_FAILED) {
            perror("sem_open failed");
            exit(1);
        }
    }

    // Initialize state array
    for (int t = 0; t < N; t++) {
        state[t] = THINKING;
    }

    pthread_mutex_lock(&print_mutex);
    printf("Philosophers State (T:Thinking, H:Hungry, E:Eating)\n");
    printf("----------------------------------------\n");
    pthread_mutex_unlock(&print_mutex);

    printf("Press Ctrl+C to exit safely\n");

    // Create threads with separate memory for each ID
    for (int t = 0; t < N; t++) {
        int *arg = malloc(sizeof(int));
        *arg = t;
        if (pthread_create(&thread[t], NULL, philosopher, arg) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }

    // Wait for threads to finish
    for (int t = 0; t < N; t++) {
        pthread_join(thread[t], NULL);
    }

    // Cleanup
    printf("Cleaning up semaphores...\n");
    sem_close(waittype);
    sem_unlink("/waittype");

    for (int t = 0; t < N; t++) {
        char sem_name[20];
        snprintf(sem_name, sizeof(sem_name), "/posttype%d", t);
        sem_close(posttype[t]);
        sem_unlink(sem_name);
    }

    pthread_mutex_destroy(&print_mutex);
    printf("Program terminated safely\n");
    return 0;
}
