#ifndef CIRCULAR_BUFFER_HH
#define CIRCULAR_BUFFER_HH

#include <stdlib.h>
#include <vector>
#include <utility>

template<typename T>
class CircularBuffer {
public:
    CircularBuffer()
	: buffer_(NULL), max_size_(0),
	  head_(0), tail_(0), size_(0), full_(false) {
    };

    ~CircularBuffer() {
	free(buffer_);
    }

    void reset() {
	reset(max_size_);
    }

    void reset(size_t size) {
	head_ = 0;
	tail_ = 0;
	size_ = 0;
	full_ = false;
	max_size_ = size;

	free(buffer_);
	buffer_ = (T*) calloc(size, sizeof(T));
    }

    void push(T item) {
	buffer_[head_] = item;

	if (full_) {
	    tail_ = (tail_ + 1) % max_size_;
	} else {
	    size_++;
	}

	head_ = (head_ + 1) % max_size_;
	full_ = head_ == tail_;
    }

    T head() const {
	return this[head_];
    }

    T operator[](size_t index) const {
	return buffer_[(head_ - index) % max_size_];
    }

    std::vector<T> getRange(size_t start, size_t num) const {
	std::vector<T> result;

	for (size_t i = 0; i < num && i < size_; i++) {
	    result.push_back(buffer_[(tail_+start+i) % max_size_]);
	}

	return result;
    }

    std::vector<T> getRange(size_t start) const {
	return getRange(start, size_ - start - 1);
    }

    std::vector<T> getRange() const {
	return getRange(0, size_);
    }

    std::vector<std::pair<std::pair<T, T>, size_t> > getPairs() const {
	std::vector<std::pair<std::pair<T, T>, size_t> > result;

	T prev = 0;
	bool first = true;
	std::vector<T> range = getRange();

	for (int i = 0; i < range.size(); i++) {
	    T &item = range[i];

	    if (!first) {
		result.push_back(std::pair<std::pair<T, T>, size_t>(std::pair<T, T>(prev, item), i-1));
		prev = item;
	    }

	    first = false;
	}

	return result;
    }

    size_t size() const {
	return size_;
    }

private:
    T *buffer_;
    size_t head_;
    size_t tail_;
    size_t size_;
    size_t max_size_;
    bool full_;
};

#endif
