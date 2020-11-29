// MultithreadingBufferExcersise.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <conio.h>

#include <deque>
#include <vector>
#include <mutex>
#include "Buffer.h"


std::mutex mu;
std::condition_variable condition;
std::deque<float> buffer;
const unsigned int bufferSize = 100;

void producer() {

}

void consumer() {

}

//template <typename T>
void printVector(std::vector<double> *values) {
    // Figuring it out took so loong... 
    std::cout << "Current buffer batch:\n";
    for (auto &&value : *values) {
        std::cout << value << ",\t";
    }
    std::cout << "\n";
}

int main()
{
    Buffer<double> *buffer = new Buffer<double>(2, 10);

    std::printf("Size of buffer: %u\n", buffer->getCurrentSize());
    buffer->pushValue(1.2);
    buffer->pushValue(3.4);
    buffer->pushValue(4.1);
    std::printf("Size of buffer: %u\n", buffer->getCurrentSize());
    
    std::vector<double>* batch = buffer->flush();
    printVector(batch);

    delete batch;
    delete buffer;

    return 0;
}