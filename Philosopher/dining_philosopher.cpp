#include <pthread.h>

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>

// Constants
constexpr int NUM_PHILOSOPHERS = 5;

// Philosopher states using enum class for type safety
enum class PhilosopherState { THINKING, HUNGRY, EATING };

class DiningPhilosophers {
   public:
    DiningPhilosophers() { initialize(); }

    ~DiningPhilosophers() { cleanup(); }

    void run() {
        std::cout << "-----T represents THINKING, H represents HUNGRY, E "
                     "represents EATING-----\n";
        createPhilosophers();
        waitForPhilosophers();
    }

   private:
    // Prevent copying
    DiningPhilosophers(const DiningPhilosophers&) = delete;
    DiningPhilosophers& operator=(const DiningPhilosophers&) = delete;

    void initialize() {
        if (pthread_mutex_init(&mutex, nullptr) != 0) {
            throw std::runtime_error("Failed to initialize mutex");
        }

        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            if (pthread_cond_init(&conditionVariables[i], nullptr) != 0) {
                cleanup();
                throw std::runtime_error(
                    "Failed to initialize condition variable");
            }
            states[i] = PhilosopherState::THINKING;
        }
    }

    void cleanup() {
        pthread_mutex_destroy(&mutex);
        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            pthread_cond_destroy(&conditionVariables[i]);
        }
    }

    struct PhilosopherData {
        DiningPhilosophers* dp;
        int id;
    };

    void createPhilosophers() {
        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            philosopherData[i] = {this, i};
            if (pthread_create(&threads[i], nullptr, philosopherRoutine,
                               &philosopherData[i]) != 0) {
                throw std::runtime_error("Failed to create philosopher thread");
            }
        }
    }

    void waitForPhilosophers() {
        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            pthread_join(threads[i], nullptr);
        }
    }

    static int getLeft(int i) {
        return (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
    }

    static int getRight(int i) { return (i + 1) % NUM_PHILOSOPHERS; }

    void showState() const {
        std::cout << "\t\t";
        for (const auto& state : states) {
            switch (state) {
                case PhilosopherState::THINKING:
                    std::cout << " T ";
                    break;
                case PhilosopherState::HUNGRY:
                    std::cout << " H ";
                    break;
                case PhilosopherState::EATING:
                    std::cout << " E ";
                    break;
            }
        }
        std::cout << '\n';
    }

    void test(int i) {
        while (!(states[i] == PhilosopherState::HUNGRY &&
                 states[getLeft(i)] != PhilosopherState::EATING &&
                 states[getRight(i)] != PhilosopherState::EATING)) {
            pthread_cond_wait(&conditionVariables[i], &mutex);
        }
        states[i] = PhilosopherState::EATING;
        std::cout << "philosopher " << (i + 1) << " is EATING  ";
        showState();
    }

    void takeForks(int i) {
        pthread_mutex_lock(&mutex);
        states[i] = PhilosopherState::HUNGRY;
        std::cout << "philosopher " << (i + 1) << " is HUNGRY  ";
        showState();
        test(i);
        pthread_mutex_unlock(&mutex);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void putForks(int i) {
        pthread_mutex_lock(&mutex);
        states[i] = PhilosopherState::THINKING;
        std::cout << "philosopher " << (i + 1) << " is THINKING";
        showState();
        pthread_cond_signal(&conditionVariables[getLeft(i)]);
        pthread_cond_signal(&conditionVariables[getRight(i)]);
        pthread_mutex_unlock(&mutex);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    static void* philosopherRoutine(void* arg) {
        PhilosopherData* data = static_cast<PhilosopherData*>(arg);
        DiningPhilosophers& dp = *data->dp;
        int id = data->id;

        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            dp.takeForks(id);
            std::this_thread::sleep_for(std::chrono::seconds(3));
            dp.putForks(id);
        }
        return nullptr;
    }

    std::array<pthread_t, NUM_PHILOSOPHERS> threads;
    std::array<PhilosopherData, NUM_PHILOSOPHERS> philosopherData;
    std::array<PhilosopherState, NUM_PHILOSOPHERS> states;
    std::array<pthread_cond_t, NUM_PHILOSOPHERS> conditionVariables;
    pthread_mutex_t mutex;
};

int main() {
    try {
        DiningPhilosophers dp;
        dp.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
