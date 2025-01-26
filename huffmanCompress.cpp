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
    size_t decodeFileUtil(const std::string &, const std::string &, int);

    void buildTree(const std::string &text, std::map<char, std::string> &charWithCode);
    void generateCodes(Node *root, const std::string &code, std::map<char, std::string> &charWithCode);
    void freeTree(Node *node);

    int binary_to_decimal(const std::string &in);

public:
    huffmanCompress() : root(nullptr) {}

    void compressFile(const std::string &);
    void decodeFile(const std::string &);

    void compressFolder(const std::string &);
    void decodeFolder(const std::string &);

    ~huffmanCompress() { freeTree(root); };
};

void huffmanCompress::buildTree(const std::string &text, std::map<char, std::string> &charWithCode)
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
    generateCodes(root, "", charWithCode);
}

void huffmanCompress::generateCodes(Node *node, const std::string &code, std::map<char, std::string> &charWithCode)
{
    if (!node)
        return;

    generateCodes(node->left, code + "0", charWithCode);

    if (!node->left && !node->right)
    {
        node->code = code;
        charWithCode[node->ch] = code;
    }

    generateCodes(node->right, code + "1", charWithCode);
}

// for debuging
void printCodes(std::map<char, std::string> &charWithCode)
{
    for (auto pair : charWithCode)
    {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
}

// for debuging
void printCodes(std::map<std::string, char> &CodeWithChar)
{
    for (auto pair : CodeWithChar)
    {
        std::cout << pair.second << " : " << pair.first << std::endl;
    }
}

void huffmanCompress::compressFile(const std::string &inputFilePath)
{

    std::ifstream input(inputFilePath, std::ios::binary);
    if (!input.is_open())
    {
        throw std::runtime_error("Failed to open input file!");
    }
    std::ofstream output(inputFilePath + ".huff", std::ios::binary);
    if (!output.is_open())
    {
        throw std::runtime_error("Failed to open output file!");
    }

    // basic reading from file
    std::string data;
    char c;
    while (input.get(c))
    {
        data += c;
    }

    input.close();

    // map for the chars with their respective codes
    std::map<char, std::string> charWithCode;

    // build a huffman tree with the data in the file
    buildTree(data, charWithCode);

    std::string encoded_data;
    for (char c : data)
        encoded_data += charWithCode[c];

    // attaching the metadata
    std::string in = "", s = "";

    in += (char)pq.size();
    while (!pq.empty())
    {
        Node *current = pq.extractMin();
        char ch = current->ch;
        in += ch;

        int code_size = charWithCode[current->ch].size();
        in += (char)code_size;

        s = charWithCode[current->ch];
        in += s;
    }

    std::cout << encoded_data << std::endl;

    // padding the encoded data to be a multiple of 8
    int padding = (8 - encoded_data.size() % 8) % 8;
    in += (char)padding;
    std::string padded_encoded = std::string(padding, '0');
    padded_encoded += encoded_data;
    encoded_data = padded_encoded;
    while (!encoded_data.empty())
    {
        in += (char)binary_to_decimal(encoded_data.substr(0, 8));
        encoded_data = encoded_data.substr(8);
    }

    std::cout << "Size before compression: " << data.size() << " bytes" << std::endl;
    std::cout << "Size after compression: " << in.size() << " bytes" << std::endl;

    output.write(in.c_str(), in.size());

    output.close();
    // std::cout << "The Huffman tree of the file" << std::endl;
    // printCodes(charWithCode);
    std::cout << "Finished compression!" << std::endl;
}

void huffmanCompress::decodeFile(const std::string &inputFilePath)
{

    std::ifstream input(inputFilePath, std::ios::binary);
    if (!input.is_open())
    {
        throw std::runtime_error("Failed to open input file!");
    }

    std::string outputFilePath = "huff_" + inputFilePath.substr(0, inputFilePath.size() - 5);

    std::ofstream output(outputFilePath, std::ios::binary);
    if (!output.is_open())
    {
        throw std::runtime_error("Failed to open output file!");
    }

    char num_unique;
    input.get(num_unique);
    // std::cout << (int)num_unique << std::endl;

    // building a tree from the metadata
    std::map<std::string, char> CodeWithChar;
    for (size_t i = 0; i < num_unique; i++)
    {
        char ch;
        char ch_code_size;
        std::string ch_code = "";

        // input >> ch;
        input.get(ch);
        // std::cout << ch << std::endl;
        input.get(ch_code_size);

        for (size_t i = 0; i < ch_code_size; i++)
        {
            char bit;
            input >> bit;
            ch_code += bit;
        }

        CodeWithChar[ch_code] = ch;
    }

    char padding;
    input >> padding;

    std::string encoded_data;
    char byte;
    while (input.get(byte))
    {
        encoded_data += std::bitset<8>(byte).to_string();
    }

    // std::cout << (int)padding << std::endl;
    // std::cout << encoded_data << std::endl;

    // remove the padding
    encoded_data = encoded_data.substr(padding);

    std::string data_decoded = "";

    size_t i = 0;
    while (i < encoded_data.size())
    {
        std::string code = "";
        bool found = false;

        while (i < encoded_data.size())
        {
            code += encoded_data[i];
            i++;

            // check if the code exists in the map
            if (CodeWithChar.find(code) != CodeWithChar.end())
            {
                data_decoded += CodeWithChar[code];
                found = true;
                break;
            }
        }

        if (!found)
        {
            if (binary_to_decimal(code) == 0)
                break;
        }
    }

    output.write(data_decoded.c_str(), data_decoded.size());

    output.close();
    input.close();

    // Validation
    // if (data_decoded.size() == data_decoded.size()) // fix this
    // {
    //     std::cout << "Decoding successful!" << std::endl;
    // }
    // else
    // {
    //     std::runtime_error("Decoding failed");
    // }
}

std::string huffmanCompress::compressFileUtil(const std::string &inputFilePath)
{
    std::ifstream input(inputFilePath, std::ios::binary);
    if (!input.is_open())
    {
        throw std::runtime_error("Failed to open input file!");
    }

    // basic reading from file
    std::string data;
    char c;
    while (input.get(c))
    {
        data += c;
    }

    input.close();

    // map for the chars with their respective codes
    std::map<char, std::string> charWithCode;

    // build a huffman tree with the data in the file
    buildTree(data, charWithCode);

    std::string encoded_data;
    for (char c : data)
        encoded_data += charWithCode[c];

    // attaching the metadata
    std::string in = "", s = "";

    in += (char)pq.size();
    while (!pq.empty())
    {
        Node *current = pq.extractMin();
        char ch = current->ch;
        in += ch;

        int code_size = charWithCode[current->ch].size();
        in += (char)code_size;

        s = charWithCode[current->ch];
        in += s;
    }

    // include the encoded data size before the padding
    int encoded_data_size = encoded_data.size() + 1;
    std::string test;
    for (int i = 0; i < 4; ++i)
    {
        in += (char)(encoded_data_size >> (i * 8));
    }

    // padding the encoded data to be a multiple of 8
    int padding = (8 - encoded_data.size() % 8) % 8;
    in += (char)padding;
    std::string padded_encoded = std::string(padding, '0');
    padded_encoded += encoded_data;
    encoded_data = padded_encoded;
    while (!encoded_data.empty())
    {
        in += (char)binary_to_decimal(encoded_data.substr(0, 8));
        encoded_data = encoded_data.substr(8);
    }

    std::cout << "Finished compression! " << inputFilePath << std::endl;

    std::string output;
    // output += in.size();
    output += in;
    return output;
}
// the inputFilePath is the string with all the compressed code in it
size_t huffmanCompress::decodeFileUtil(const std::string &compressedData, const std::string &outputFilePath, int pos = 0)
{

    std::ofstream output(outputFilePath, std::ios::binary);
    if (!output.is_open())
    {
        throw std::runtime_error("Failed to open output file " + outputFilePath);
    }

    // size_t pos = 0;
    char num_unique = compressedData[pos];
    pos++;

    // building a tree from the metadata
    std::map<std::string, char> CodeWithChar;
    for (size_t i = 0; i < num_unique; i++)
    {
        char ch = compressedData[pos];
        pos++;

        char ch_code_size = compressedData[pos];
        pos++;

        std::string ch_code = compressedData.substr(pos, ch_code_size);
        pos += ch_code_size;

        // std::cout << "char: " << ch;
        // std::cout << ch_code << std::endl;

        CodeWithChar[ch_code] = ch;
    }

    uint32_t encoded_data_size = 0;
    for (int i = 0; i < 4; ++i)
    {
        encoded_data_size |= (static_cast<uint32_t>(compressedData[pos + i]) & 0xFF) << (i * 8);
    }
    pos += 4;

    // next multiple of 8
    encoded_data_size = (encoded_data_size + 7) / 8 * 8;

    // std::cout << "THE ENCODED SIZE: " << encoded_data_size << std::endl;

    char padding = compressedData[pos];
    // std::cout << "PADDING: " << (int)padding << std::endl;
    // pos++;

    // convert the encoded data to binary
    int to = 1 + pos + encoded_data_size / 8;
    // std::cout << "TO WHERE i: " << to << std::endl;
    std::string encoded_data;
    while (pos < to)
    {
        char byte = compressedData[pos];
        // std::cout << "BYTE AFTER : " << byte << std::endl;
        pos++;

        encoded_data += std::bitset<8>(byte).to_string();
    }

    // remove the padding
    encoded_data = encoded_data.substr(8);
    encoded_data = encoded_data.substr(padding);

    std::string data_decoded = "";

    size_t i = 0;
    while (i < encoded_data.size())
    {
        std::string code = "";
        bool found = false;

        while (i < encoded_data.size())
        {
            code += encoded_data[i];
            i++;

            // check if the code exists in the map
            if (CodeWithChar.find(code) != CodeWithChar.end())
            {
                data_decoded += CodeWithChar[code];
                found = true;
                break;
            }
        }

        if (!found)
        {
            if (binary_to_decimal(code) == 0)
                break;
        }
    }

    // std::cout << data_decoded << std::endl;

    output.write(data_decoded.c_str(), data_decoded.size());

    output.close();

    return pos;
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

void huffmanCompress::compressFolder(const std::string &inputFolder)
{
    std::string header;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(inputFolder))
    {
        std::string relative_path = std::filesystem::relative(entry.path(), inputFolder).string();

        if (entry.is_regular_file())
        {
            // can actually make it include just the name of the file and then when decomp just use how many files are in the folder
            std::string compressed_data = compressFileUtil(entry.path().string());

            header += ">" + relative_path + "|";
            header += compressed_data;
        }
        else if (entry.is_directory())
        {
            header += "<";
            header += relative_path + "|";
        }
    }

    std::ofstream outFile(inputFolder + ".huff", std::ios::binary);
    outFile.write(header.c_str(), header.size());
    outFile.close();

    std::cout << "Finished compressing" << std::endl;
}

void huffmanCompress::decodeFolder(const std::string &inputFolder)
{
    std::ifstream input(inputFolder, std::ios::binary);
    if (!input)
    {
        throw std::runtime_error("Failed to open compressed file for reading.");
    }

    std::string folderName = "huff_" + inputFolder.substr(0, inputFolder.size() - 5);
    std::filesystem::create_directories(folderName);

    std::string compressed_data;
    char c;
    while (input.get(c))
    {
        compressed_data += c;
    }

    int pos = 0;
    while (pos < compressed_data.size())
    {
        char type = compressed_data[pos];
        pos++;

        size_t pathEnd = compressed_data.find('|', pos);
        // some check if there is an error in the compression

        std::string relativePath = compressed_data.substr(pos, pathEnd - pos);
        pos = pathEnd + 1;

        std::string fullpath = folderName + "\\" + relativePath;
        std::cout << fullpath << std::endl;

        // dictionary
        if (type == '<')
        {
            std::filesystem::create_directories(fullpath);
        }
        if (type == '>')
        {
            pos = decodeFileUtil(compressed_data, fullpath, pos);
        }
    }
}

int main()
{
    huffmanCompress h;
    // h.buildTree("AAABBBBBCCCCCCDDDDEE");

    // h.printCodes();
    // h.compressFile("brah.txt");
    // h.decodeFile("brah.txt.huff");

    // h.compressFolder("scrape");
    h.decodeFolder("scrape.huff");

    return 0;
}
