#pragma once
#include <deque>
#include <vector>

template <typename T>
class Buffer
{
private:
	std::deque<T> container;
	unsigned int maxSize;
	unsigned int batchSize;

	unsigned int getCurrentBatchSize() {
		return container.size() > batchSize ? batchSize : container.size();
	}

public:
	Buffer<T>(int unsigned maxSize, int unsigned batchSize = 10) : maxSize(maxSize), batchSize(batchSize)
	{ }

	unsigned int getCurrentSize() {
		return container.size();
	}

	void pushValue(T value) {
		if (container.size() == maxSize) {
			container.pop_back();
		}
		container.push_front(value);
	}

	std::vector<T>* flush() {
		std::vector<T>* batch = new std::vector<T>;

		// If the buffer is empty returns empty
		if (container.empty()) return batch;

		unsigned int currentBatchSize = getCurrentBatchSize();
		typename std::deque<T>::iterator it;
		for (it = container.begin(); it != (container.begin() + currentBatchSize); it++) {
			batch->push_back(*it);
		}

		container.erase(container.begin(), container.begin() + currentBatchSize);

		return batch;
	}
};

