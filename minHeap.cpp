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

template <typename T, typename Comparator>
void minHeap<T, Comparator>::buildHeap(const std::vector<int> &other)
{
    data = other;
    for(int i = data.size()/2-1; i>=0;i--)
        soak(i);
}

template <typename T, typename Comparator>
void minHeap<T, Comparator>::insert(T el)
{
    data.push_back(el);
    heapify(data.size()-1);
}

template <typename T, typename Comparator>
T minHeap<T, Comparator>::extractMin()
{
    if (data.empty())
        throw std::underflow_error("Heap is empty");
    
    T root = data[0];
    data[0] = data[data.size() - 1];
    data.pop_back();
    
    soak(0);

    return root;
}

template <typename T, typename Comparator>
void minHeap<T, Comparator>::heapify(int index)
{
    while (index != 0 && comp(data[index], data[parent(index)]))
    {
        std::swap(data[index], data[parent(index)]);
        index = parent(index);
    }
}


template <typename T, typename Comparator>
void minHeap<T, Comparator>::soak(int index)
{
    int l = left(index);
    int r = right(index);
    int smallest = index;

    if ((l < data.size()) && comp(data[l], data[smallest]))
        smallest = l;
    if ((r < data.size()) && comp(data[r], data[smallest]))
        smallest = r;

    if (smallest != index)
    {
        std::swap(data[index], data[smallest]);
        soak(smallest);
    }
}

template <typename T, typename Comparator>
void minHeap<T, Comparator>::print() const
{
    int pow = 0;
    int val = 1;
    for (size_t i = 0; i < data.size(); i++)
    {
        if (i == val)
        {
            std::cout << std::endl;
            pow++;
            val += (1 << pow);
        }
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;
}

template <typename T, typename Comparator>
size_t minHeap<T, Comparator>::size() const
{
    return data.size();
}

template <typename T, typename Comparator>
bool minHeap<T, Comparator>::empty() const
{
    return data.empty();
}

// int main()
// {
//     int N = 15;

//     minHeap<int> data(N);

//     for (size_t i = 0; i < N; i++)
//     {
//         data.insert(std::rand() % 100);
//         data.print();
//         std::cout << std::endl;
//     }

//     return 0;
// }