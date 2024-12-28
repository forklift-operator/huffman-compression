#include <iostream>
#include <vector>
#include <algorithm>

template <typename T>
class minHeap
{
private:
    int size;
    int cap;
    std::vector<T> heap;

    int parent(int index) { return (index - 1) / 2; };
    int left(int index) { return (index * 2) + 1; };
    int right(int index) { return (index * 2) + 2; };

public:
    minHeap(int cap);

    void insert(T val);
    int extractMin();
    void soak(int index);
    void heapify(int index);
    void print() const;

    ~minHeap() {};
};

template <typename T>
minHeap<T>::minHeap(int cap)
{
    size = 0;
    this->cap = cap;
    heap.resize(cap);
}

template <typename T>
void minHeap<T>::insert(T val)
{
    if (size == cap)
    {
        std::cout << "MINHEAP FULL!" << std::endl; // make this throw an error
        return;
    }

    size++;

    int i = size - 1;
    heap[i] = val;
    soak(i);
    }

template <typename T>
int minHeap<T>::extractMin()
{
    if (size == 0)
    {
        std::cout << "MINHEAP IS EMPTY" << std::endl;
        return -1;
    }
    else if (size == 1)
    {
        size--;
        return heap[0];
    }
    else
    {
        int root = heap[0];

        heap[0] = heap[size - 1];
        size--;
        heapify(0);

        return root;
    }
}

template <typename T>
void minHeap<T>::soak(int index)
{
    while (index != 0 && heap[parent(index)] > heap[index])
    {
        std::swap(heap[index], heap[parent(index)]);
        index = parent(index);
    }
}

template <typename T>
void minHeap<T>::heapify(int index)
{
    int l = left(index);
    int r = right(index);
    int smallest = index;

    if ((l < size) && (heap[l] < heap[smallest]))
        smallest = l;
    if ((r < size) && (heap[r] < heap[smallest]))
        smallest = r;

    if (smallest != index)
    {
        std::swap(heap[index], heap[smallest]);
        heapify(smallest);
    }
}

template <typename T>
void minHeap<T>::print() const
{
    int pow = 0;
    int val = 1;
    for (size_t i = 0; i < size; i++)
    {
        if (i == val)
        {
            std::cout << std::endl;
            pow++;
            val += (1 << pow);
        }
        std::cout << heap[i] << " ";
    }
    std::cout << std::endl;
}

int main()
{
    int N = 15;

    minHeap<int> heap(N);

    for (size_t i = 0; i < N; i++)
    {
        char randomChar = 'A' + std::rand() % 26;
        heap.insert(randomChar);
        heap.print();
        std::cout << std::endl;
    }

    return 0;
}