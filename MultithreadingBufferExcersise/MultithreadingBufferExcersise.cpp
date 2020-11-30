// MultithreadingBufferExcersise.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <conio.h>
#include <deque>
#include <vector>
#include <mutex>
#include <random>

#include "Buffer.hpp"

static std::random_device rd;
static std::mt19937 gen(rd());

// This flag might not be the best way to control these threads
bool shouldRun = true;

// Configuration
const unsigned int BUFFER_SIZE = 20;
const unsigned int BATCH_SIZE = 8;

template <typename T>
void printVector(std::vector<T>* values) {
    std::cout << "Current buffer batch:\n";
    // Figuring out how to make it work with pointer took me way too long... 
    for (auto&& value : *values) {
        std::cout << value << ",\t";
    }
    std::cout << "\n";
};

double getRandomNumber() {
    std::uniform_real_distribution<> dis(0, 100);
    return std::sin(dis(gen));
}

void producer(std::shared_ptr<Buffer<double>> buffer, bool verbal = false) {
    while (shouldRun) {

        // I wasn't sure if sin(x) form the description meant some sort of rnumb
        // or sin(times_of_execution). Random number avoids overflowing loop counter
        double rvalue = getRandomNumber();

        if(verbal)
        std::printf("Producer: Random signal %f\n", rvalue);

        buffer->pushValue(rvalue);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer(std::shared_ptr<Buffer<double>> buffer, unsigned int sleepTime = 2, bool displayBatch = false) {
    while (shouldRun) {
        std::vector<double>* batch = buffer->fetch();

        // Not sure if consure is responsible for showing the buffer size as it
        // technically only see last batch that was available to it
        // I have made assumption that there it's not required to create seperate
        // thread just for displaying entier buffer not
        if (displayBatch) {
            // It's such a pain that you can't convert in one line thread id
            // into a c_str, I didn't wanted to use C++20 std::format as it's too fresh
            std::cout << "Consumer | thread id: " << std::this_thread::get_id()
                << "  size of the batch: " << batch->size() << std::endl;
            printVector<double>(batch);
        }

        delete batch;

        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    }
}

int main()
{
    // Requirements say that implementation should allow for only one instance
    // Easiest impementation is to use singletion but it has multiple drawbacks
    // Anternatively we could use service locator or DI (will be added if I have enough time)
    std::shared_ptr<Buffer<double>> buffer(new Buffer<double>(BUFFER_SIZE, BATCH_SIZE));

    std::cout << "Press any key to stop threads...\n";

    // Not sure how to use optional parameters with threads
    std::thread t1(producer, std::ref(buffer), false);
    std::thread t2(consumer, buffer, 5, true);
    // If these would have the same parameters we could simply implement thread pool
    // with parameters it would require some sort of configuration handling
    std::thread t3(consumer, buffer, 1, true);

    char c =_getch();
    shouldRun = false;

    t1.join();
    t2.join();
    t3.join();

    std::printf("Size of the buffer: %u\n", buffer->getCurrentSize());


    return 0;
}