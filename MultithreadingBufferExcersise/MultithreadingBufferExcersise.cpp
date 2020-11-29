// MultithreadingBufferExcersise.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <conio.h>
#include <deque>
#include <vector>
#include <mutex>
#include <random>

#include "Buffer.hpp"

static std::random_device rd;
static std::mt19937 gen(rd());

std::mutex mu;
std::condition_variable condition;
std::deque<float> buffer;
bool shouldRun = true;
const unsigned int BUFFER_SIZE = 20;
const unsigned int BATCH_SIZE = 10;

//template <typename T>
void printVector(std::vector<double>* values) {
    // Figuring it out took so loong... 
    std::cout << "Current buffer batch:\n";
    for (auto&& value : *values) {
        std::cout << value << ",\t";
    }
    std::cout << "\n";
}

double getRandomNumber() {
    std::uniform_real_distribution<> dis(0, 100);
    return std::sin(dis(gen));
}

void producer(std::shared_ptr<Buffer<double>> buffer) {
    while (shouldRun) {

        std::printf("Producer: Size of the buffer %u\n", buffer->getCurrentSize());
        // I wasn't sure if sin(x) form the description meant some sort of rnumb
        // or sin(times_of_execution). Random number avoids overflowing loop counter
        double rvalue = getRandomNumber();
        std::printf("Producer: Random signal %f\n", rvalue);

        buffer->pushValue(rvalue);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer(std::shared_ptr<Buffer<double>> buffer) {
    while (shouldRun) {
        std::printf("Consumer: Size of the whole buffer: %u\n", buffer->getCurrentSize());

        std::vector<double>* batch = buffer->flush();
        std::printf("Consumer: Size of the batch: %u\n", batch->size());
        printVector(batch);
        delete batch;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main()
{
    // Requirements say that implementation should allow for only one instance
    // Easiest impementation is to use singletion but it has multiple drawbacks
    // Anternatively we could use service locator or DI (will be added if I have enough time)
    std::shared_ptr<Buffer<double>> buffer(new Buffer<double>(BUFFER_SIZE, BATCH_SIZE));

    std::cout << "Press any key to stop threads...\n";


    std::thread t1(producer, buffer);
    std::thread t2(consumer, buffer);

    _getch();
    shouldRun = false;

    t1.join();
    t2.join();

    std::printf("Size of the buffer: %u\n", buffer->getCurrentSize());


    return 0;
}