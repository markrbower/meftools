// From: https://softwarepatterns.com/cpp/iterator-software-pattern-cpp-example

/* Usage:
int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    Iterator<int>* iterator = new VectorIterator<int>(numbers);

    while (iterator->hasNext()) {
        std::cout << iterator->next() << " ";
    }

    delete iterator;

    return 0;
}
*/


#include <iostream>
#include <vector>

#include "VectorIterator.h"

// Iterator interface
/*
template <typename T>
class Iterator {
public:
    virtual T next() = 0;
    virtual bool hasNext() = 0;
};
*/

// Concrete Iterator for a vector
template <typename T>
class VectorIterator : public Iterator<T> {
public:
    VectorIterator(std::vector<T>& vec) : vector_(vec), index_(0) {}

    T next() override {
        return vector_[index_++];
    }

    bool hasNext() override {
        return index_ < vector_.size();
    }

private:
    std::vector<T>& vector_;
    size_t index_;
};

