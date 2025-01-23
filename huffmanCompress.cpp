#include <string>
#include <vector>
#include <iostream>
#include "minHeap.cpp"
#include <bitset>
#include <fstream>
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
    std::map<char, std::string> charWithCode;
    minHeap<Node *, CompareNode> pq;

    void generateCodes(Node *root, const std::string &code);
    void freeTree(Node *node);

    int binary_to_decimal(const std::string &in);
    std::string decimal_to_binary(int in, int bit_length = 0);

public:
    huffmanCompress() : root(nullptr) {} // make a constructor with the msg and inside of it do all the logic of building the tree generating the codes

    void buildTree(const std::string text);

    void compressFile(const std::string &inputFile, const std::string &outputFilePath);
    void decode(const std::string &, const std::string &);

    void printCodes();

    ~huffmanCompress() { freeTree(root); }; // stack overflow
};

void huffmanCompress::buildTree(const std::string text)
{
    // set the freqs
    std::vector<int> freq(256, 0);
    for (char c : text)
    {
        freq[c]++;
    }

    // initial nodes
    for (int i = 0; i < freq.size(); i++)
    {
        if (freq[i] != 0)
        {
            pq.insert(new Node((char)i, freq[i]));
        }
    }
    // the above could be done in a different function "create pq"

    // make the huff tree
    minHeap<Node *, CompareNode> huffmanTree(pq);
    while (huffmanTree.size() != 1)
    {
        Node *left = huffmanTree.extractMin();
        Node *right = huffmanTree.extractMin();

        // fix for when the new Huffman nodes are being inserted
        if (!left->left && right->left)
        {
            Node *temp = left;
            left = right;
            right = temp;
        }

        int sum_freq = left->freq + right->freq;
        huffmanTree.insert(new Node(' ', sum_freq, left, right));
    }

    // generate the codes from the root of the huffman tree
    root = huffmanTree.extractMin();
    generateCodes(root, "");
}

void huffmanCompress::generateCodes(Node *node, const std::string &code)
{
    if (!node)
        return;

    generateCodes(node->left, code + "0");

    if (!node->left && !node->right)
    {
        node->code = code;
        charWithCode[node->ch] = code;
    }

    generateCodes(node->right, code + "1");
}

// make this to the tree class and fix name
int next_multiple_of_eight(int n)
{
    return (n + 7) & ~7;
}

void huffmanCompress::compressFile(const std::string &inputFilePath, const std::string &outputFilePath)
{

    std::ifstream input(inputFilePath, std::ios::binary);
    if (!input.is_open())
    {
        std::cout << "Failed to open file: " << inputFilePath << std::endl;
        return;
    }
    std::ofstream output(outputFilePath, std::ios::binary);
    if (!output.is_open())
    {
        std::cout << "Failed to open file: " << outputFilePath << std::endl;
        return;
    }

    // Basic reading from file
    std::string data;
    char c;
    while (input.get(c))
    {
        data += c;
    }

    input.close();

    // Build a huffman tree with the data in the file
    buildTree(data);

    std::string encodedData;
    for (char c : data)
        encodedData += charWithCode[c];

    // Attaching the metadata
    std::string in = "", s = "";

    in += (char)pq.size(); // the first byte saves the size of the priority queue
    minHeap<Node *, CompareNode> temp(pq);
    while (!temp.empty())
    { // get all characters and their huffman codes for output
        Node *current = temp.extractMin();
        in += current->ch;
        in += (char)current->code.size();

        s.assign(next_multiple_of_eight(pq.size() - 1) - current->code.size(), '0');
        s += '1';
        s.append(current->code);

        in += (char)binary_to_decimal(s); // needs to account for the cases where the s is more than 8 bits with below approach
        // in += (char)binary_to_decimal(s.substr(0, 8));
        s = s.substr(8);
    }
    s.clear();

    in += (char)data.size();
    in += (char)encodedData.size();
    while (!encodedData.empty())
    {
        if (encodedData.size() < 8)
        {
            encodedData.append(8 - encodedData.size(), '0');
        }
        in += (char)binary_to_decimal(encodedData.substr(0, 8));
        encodedData = encodedData.substr(8);
    }

    std::cout << "Size before compression: " << data.size() << " bytes" << std::endl;
    std::cout << "Size after compression: " << in.size() << " bytes" << std::endl;

    output.write(in.c_str(), in.size());

    output.close();

    // std::cout << "The Huffman tree of the file" << std::endl;
    // printCodes();
}

void huffmanCompress::decode(const std::string &inputFilePath, const std::string &outputFilePath)
{
    std::ifstream input(inputFilePath, std::ios::binary);
    if (!input.is_open())
    {
        std::cout << "Failed to open file: " << inputFilePath << std::endl;
        return;
    }

    std::ofstream output(outputFilePath, std::ios::binary);
    if (!output.is_open())
    {
        std::cout << "Failed to open file: " << outputFilePath << std::endl;
        return;
    }

    unsigned char size;
    input.read((char *)&size, 1);

    std::map<std::string, char> CodeWithChar;
    // building a tree
    for (size_t i = 0; i < size; i++)
    {
        char ch, ch_size, ch_code;
        input.read(&ch, 1);
        input.read(&ch_size, 1);
        input.read(&ch_code, 1);

        int actual_code = ch_code ^ (1 << ch_size);
        int bit_length = (unsigned char)ch_size;
        std::string code = decimal_to_binary(actual_code, bit_length);
        CodeWithChar[code] = ch;
    }

    unsigned char data_size, usable_bits;
    input.read((char *)&data_size, 1);
    input.read((char *)&usable_bits, 1);

    std::string encodedData;
    char byte;
    while (input.get(byte))
    {
        encodedData += std::bitset<8>(byte).to_string();
    }
    // std::cout << encodedData << std::endl;

    std::string data_decoded = "";
    for (size_t i = 0; i < data_size;)
    {
        std::string code = "";
        for (size_t j = i; j < usable_bits; j++)
        {
            code += encodedData[j];
            if (CodeWithChar.find(code) != CodeWithChar.end())
            {
                data_decoded += CodeWithChar[code];
                i += code.size();
                code.clear();
            }
        }
    }


    output.write(data_decoded.c_str(), data_decoded.size());

    output.close();
    input.close();
    
    if (data_decoded.size() == data_size)
    {
        std::cout << "Decoding successful!" << std::endl;
    }
    else
    {
        std::runtime_error("Decoding failed");
    }
}

void huffmanCompress::printCodes()
{
    for (auto pair : charWithCode)
    {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
}

void huffmanCompress::freeTree(Node *node)
{
    if (!node)
        return;
    if (node->left)
        freeTree(node->left);
    if (node->right)
        freeTree(node->right);
    delete node;
}

int huffmanCompress::binary_to_decimal(const std::string &in)
{
    int result = 0;
    for (int i = 0; i < in.size(); i++)
        result = result * 2 + in[i] - '0';
    return result;
}

std::string huffmanCompress::decimal_to_binary(int in, int bit_length)
{
    std::string result = "";
    while (in)
    {
        result = (char)('0' + (in % 2)) + result;
        in /= 2;
    }
    
    if (bit_length > 0 && result.size() < bit_length)
    {
        result = std::string(bit_length - result.size(), '0') + result;
    }
    return result;
}

int main()
{
    huffmanCompress h;
    // h.buildTree("AAABBBBBCCCCCCDDDDEE");

    // h.printCodes();
    h.compressFile("test.txt", "test.huff");
    h.decode("test.huff", "test_decoded.txt");
    return 0;
}