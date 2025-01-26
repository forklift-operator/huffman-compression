#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "minHeap.cpp"
#include <bitset>
#include <fstream>
#include <filesystem>
#include <map>
#include <unordered_map>
#include <stdexcept>
struct Node
{
    int freq;
    Node *left;
    Node *right;
    std::string code;
    char ch = 0;
    Node(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}
    Node(char ch, int freq, Node *left, Node *right) : ch(ch), freq(freq), left(left), right(right) {}
};

struct CompareNode
{
    bool operator()(const Node *lhs, const Node *rhs) const
    {
        return lhs->freq < rhs->freq;
    }
};

class huffmanCompress
{
private:
    Node *root;
    minHeap<Node *, CompareNode> pq;

    std::string compressFileUtil(const std::string &);
    size_t decompressFileUtil(const std::string &, const std::string &, int);

    void buildTree(const std::string &text, std::map<char, std::string> &charWithCode);
    void generateCodes(Node *root, const std::string &code, std::map<char, std::string> &charWithCode);
    void freeTree(Node *node);

    int binary_to_decimal(const std::string &in);
public:
    huffmanCompress() : root(nullptr) {}

    void compressFile(const std::string &);
    void decompressFile(const std::string &);

    void compressFolder(const std::string &);
    void decompressFolder(const std::string &);

    void info(const std::string &);

    ~huffmanCompress() { freeTree(root); };
};
