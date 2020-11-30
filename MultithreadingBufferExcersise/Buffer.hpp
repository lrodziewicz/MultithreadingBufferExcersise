#pragma once
#include <deque>
#include <vector>
#include <shared_mutex>

// I have spent so much more time then it's been needed just by using templates
// for this implementation
template <typename T>
class Buffer
{
private:
	std::deque<T> container;
	mutable std::shared_mutex mu;
	std::condition_variable condition;

	unsigned int maxSize;
	unsigned int batchSize;

	unsigned int getCurrentBatchSize() {
		return container.size() > batchSize ? batchSize : container.size();
	}

public:
	Buffer<T>(int unsigned maxSize, int unsigned batchSize = 10) : maxSize(maxSize), batchSize(batchSize)
	{ }

	~Buffer() {
		// Everything should be cleared automatically... I think...
	}

	unsigned int getCurrentSize() {
		std::shared_lock<std::shared_mutex> lock(mu);
		return container.size();
	}

	// This implementation assumes that by "lewa strona" you meant front and vice versa
	void pushValue(T value) {
		// I guess since C++17 we should use std::scoped_lock instead
		std::lock_guard<std::shared_mutex> lock(mu);

		// I'm not sure if I understood requirements correctly but in
		// some cases we can use condition_variable to put other threads into sleep
		// in case of one producer I don't think it's nessesary
		//condiation.wait(lock, [this]() { return container.size() < batchSize; })

		if (container.size() == maxSize) {
			container.pop_back();
		}
		container.push_front(value);
		// Unlock is called once lock falls out of scope
	}

	std::vector<T>* fetch() {
		std::vector<T>* batch = new std::vector<T>;

		// If the buffer is empty returns empty
		if (container.empty()) return batch;

		// As there is multiple consumers we probably could use
		// condition_variable here but I'm not sure if it makes sense
		// if buffer is not flushed when consumed (that was my understanding)
		std::shared_lock<std::shared_mutex> lock(mu);
		std::cout << "Current total buffer size: "
			<< container.size()
			<< std::endl;

		unsigned int currentBatchSize = getCurrentBatchSize();
		typename std::deque<T>::iterator it;
		for (it = container.begin(); it != (container.begin() + currentBatchSize); it++) {
			batch->push_back(*it);
		}

		// I'm not sure why but I initially assumed that consuming buffer will clear
		// entier batch from the buffer
		//container.erase(container.begin(), container.begin() + currentBatchSize);

		return batch;
	}
};

