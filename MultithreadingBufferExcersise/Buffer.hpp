#pragma once
#include <deque>
#include <vector>

// I have spent so much more time then it's been needed just by using templates
// for this implementation
template <typename T>
class Buffer
{
private:
	std::deque<T> container;
	std::mutex mu;
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
		return container.size();
	}

	// This implementation assumes that by "lewa strona" you meant front and vice versa
	void pushValue(T value) {
		// I guess since C++17 we should use std::scoped_lock instead
		std::lock_guard<std::mutex> lock(mu);
		if (container.size() == maxSize) {
			container.pop_back();
		}
		container.push_front(value);
		// Unlock is called once lock falls out of scope
	}

	std::vector<T>* flush() {
		std::vector<T>* batch = new std::vector<T>;

		// If the buffer is empty returns empty
		if (container.empty()) return batch;

		std::lock_guard<std::mutex> lock(mu);
		unsigned int currentBatchSize = getCurrentBatchSize();
		typename std::deque<T>::iterator it;
		for (it = container.begin(); it != (container.begin() + currentBatchSize); it++) {
			batch->push_back(*it);
		}

		container.erase(container.begin(), container.begin() + currentBatchSize);

		return batch;
	}
};

