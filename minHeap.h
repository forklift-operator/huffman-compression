#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

template <typename T, typename Comparator = std::less<T>>
class minHeap
{
private:
    std::vector<T> data;
    Comparator comp;

    int parent(int index) { return (index - 1) / 2; };
    int left(int index) { return (index * 2) + 1; };
    int right(int index) { return (index * 2) + 2; };

public:
    minHeap(){};
    minHeap(size_t size) {data.reserve(size);};
    minHeap(const std::vector<int>& other) {buildHeap(other);};

    void buildHeap(const std::vector<int>& other);
    void insert(T val);
    T extractMin();
    void soak(int index);
    void heapify(int index);
    void print() const;
    size_t size() const;
    bool empty() const;

    ~minHeap() {};
};