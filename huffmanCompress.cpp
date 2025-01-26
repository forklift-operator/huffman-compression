#include "huffmanCompress.h"

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

    if (data.empty())
    {
        std::ofstream output(inputFilePath + ".huff", std::ios::binary);
        if (!output.is_open())
        {
            throw std::runtime_error("Failed to open output file!");
        }

        std::string emptyMarker(1, 0); // num_unique = 0
        emptyMarker += (char)0;        // padding = 0
        output.write(emptyMarker.c_str(), emptyMarker.size());
        output.close();
        return;
    }

    // map for the chars with their respective codes
    std::map<char, std::string> charWithCode;

    // build a huffman tree with the data in the file
    buildTree(data, charWithCode);

    // check if there is just one type of char in the file
    if (charWithCode.size() == 1)
    {
        auto &entry = *charWithCode.begin();
        if (entry.second.empty())
        {
            entry.second = "0"; // Fix empty code
        }
    }

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

    output.write(in.c_str(), in.size());
    output.close();

    std::cout << "Compressed: " + inputFilePath << std::endl;
    std::cout << "Size before compression: " << data.size() << " bytes" << std::endl;
    std::cout << "Size after compression: " << in.size() << " bytes" << std::endl
              << std::endl;
}

void huffmanCompress::decompressFile(const std::string &inputFilePath)
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
    // if the file is empty
    if (num_unique == 0)
    {
        std::string outputFilePath = "huff_" + inputFilePath.substr(0, inputFilePath.size() - 5);
        std::ofstream output(outputFilePath, std::ios::binary);
        if (!output.is_open())
        {
            throw std::runtime_error("Failed to create empty output file!");
        }

        output.close();
        return;
    }

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

    std::string data_decompressd = "";

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
                data_decompressd += CodeWithChar[code];
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

    output.write(data_decompressd.c_str(), data_decompressd.size());

    output.close();
    input.close();

    std::cout << "Decompressed: " << outputFilePath << std::endl;

    // Validation
    // if (data_decompressd.size() == data_decompressd.size()) // fix this
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

    if (data.empty())
    {
        std::string output;
        output += (char)0;
        return output;
    }

    input.close();

    // map for the chars with their respective codes
    std::map<char, std::string> charWithCode;

    // build a huffman tree with the data in the file
    buildTree(data, charWithCode);

    // check if there is just one type of char in the file
    if (charWithCode.size() == 1)
    {
        auto &entry = *charWithCode.begin();
        if (entry.second.empty())
        {
            entry.second = "0";
        }
    }

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

    // original size of the file for validation (8 bytes)
    // uint64_t original_size = data.size();
    // for (int i = 0; i < 8; ++i)
    // {
    //     in += (char)(original_size >> (i * 8));
    // }

    // include the encoded data size including the padding (4 bytes)
    uint32_t encoded_data_size = encoded_data.size() + 1;
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

    std::cout << "Compressed: " << inputFilePath << std::endl;

    return in;
}
// the inputFilePath is the string with all the compressed code in it
size_t huffmanCompress::decompressFileUtil(const std::string &compressedData, const std::string &outputFilePath, int pos = 0)
{

    std::ofstream output(outputFilePath, std::ios::binary);
    if (!output.is_open())
    {
        throw std::runtime_error("Failed to open output file " + outputFilePath);
    }

    // size_t pos = 0;
    char num_unique = compressedData[pos];
    pos++;

    if (num_unique == 0)
    {
        output.close();
        return pos;
    }

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

        CodeWithChar[ch_code] = ch;
    }

    uint32_t encoded_data_size = 0;
    for (int i = 0; i < 4; ++i)
    {
        encoded_data_size |= (static_cast<uint32_t>(compressedData[pos + i]) & 0xFF) << (i * 8);
    }
    pos += 4;

    // read the original size of the file for validation
    // uint64_t original_size = 0;
    // for (int i = 0; i < 8; ++i)
    // {
    //     original_size |= (static_cast<uint32_t>(compressedData[pos + i]) & 0xFF) << (i * 8);
    // }
    // pos += 8;

    // next multiple of 8
    encoded_data_size = (encoded_data_size + 7) / 8 * 8;

    char padding = compressedData[pos];

    int to = 1 + pos + encoded_data_size / 8;
    std::string encoded_data;
    while (pos < to)
    {
        char byte = compressedData[pos];
        pos++;

        encoded_data += std::bitset<8>(byte).to_string();
    }

    // remove the padding
    encoded_data = encoded_data.substr(8);
    encoded_data = encoded_data.substr(padding);

    std::string data_decompressd = "";

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
                data_decompressd += CodeWithChar[code];
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

    // if (original_size == data_decompressd.size()+1)
    // {
    //     std::cout << "Decoding successful! " << std::endl;
    // }
    // else
    // {
    //     // throw std::runtime_error("Decoding failed for " + outputFilePath);
    // }

    output.write(data_decompressd.c_str(), data_decompressd.size());
    output.close();

    std::cout << "Decompressed: " << outputFilePath << std::endl;

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

    uint64_t size = 0;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(inputFolder))
    {
        std::string relative_path = std::filesystem::relative(entry.path(), inputFolder).string();

        if (entry.is_regular_file())
        {
            std::string compressed_data = compressFileUtil(entry.path().string());

            header += ">" + relative_path + "|";
            header += compressed_data;

            size += std::filesystem::file_size(entry.path());
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

    uint64_t newSize = std::filesystem::file_size(inputFolder + ".huff");

    std::cout << "Compression complete! " << std::endl;
    std::cout << "Size before compression: " << size << " bytes" << std::endl;
    std::cout << "Size after compression: " << newSize << " bytes" << std::endl
              << std::endl;
}

void huffmanCompress::decompressFolder(const std::string &inputFolder)
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

        // dictionary
        if (type == '<')
        {
            std::filesystem::create_directories(fullpath);
        }
        if (type == '>')
        {
            pos = decompressFileUtil(compressed_data, fullpath, pos);
        }
    }
    std::cout << "Decompression complete! " << std::endl
              << std::endl;
}

void huffmanCompress::info(const std::string &inputFilePath)
{
    std::ifstream input(inputFilePath, std::ios::binary);
    if (!input)
    {
        throw std::runtime_error("Failed to open compressed file for reading.");
    }

    std::string compressed_data;
    char c;
    while (input.get(c))
    {
        compressed_data += c;
    }

    std::cout << "Info for: " << inputFilePath << "\n";
    std::cout << "----------------------------------------\n";

    int pos = 0;
    while (pos < compressed_data.size())
    {
        char type = compressed_data[pos];
        pos++;

        size_t pathEnd = compressed_data.find('|', pos);
        if (pathEnd == std::string::npos)
        {
            throw std::runtime_error("Invalid compressed file format.");
        }

        std::string relativePath = compressed_data.substr(pos, pathEnd - pos);
        pos = pathEnd + 1;

        if (type == '<')
        {
            std::cout << "[Dir] " << relativePath << "\n";
        }
        else if (type == '>')
        {
            std::cout << "[File] " << relativePath << "\n";

            // Extract the compressed data size
            uint32_t encoded_data_size = 0;
            for (int i = 0; i < 4; ++i)
            {
                encoded_data_size |= (static_cast<uint32_t>(compressed_data[pos + i]) & 0xFF) << (i * 8);
            }
            pos += 4;

            // Skip the padding and compressed data
            char padding = compressed_data[pos];
            pos++;

            int to = pos + (encoded_data_size + 7) / 8;
            pos = to;

            std::cout << "  Compressed Size: " << (encoded_data_size + 7) / 8 << " bytes\n";
        }
    }

    std::cout << "----------------------------------------\n";
    std::cout << "Total Compressed File Size: " << compressed_data.size() << " bytes\n";
}

void displayMenu()
{
    std::cout << "Huffman Compression\n";
    std::cout << "1. Compress File\n";
    std::cout << "2. Decompress File\n";
    std::cout << "3. Compress Folder\n";
    std::cout << "4. Decompress Folder\n";
    std::cout << "5. Info\n";
    std::cout << "6. Exit\n";
    std::cout << "Enter your choice: ";
}

void handleUserChoice(int choice, huffmanCompress &h)
{
    std::string path;
    switch (choice)
    {
    case 1:
        std::cout << "Enter the file path to compress: ";
        std::cin >> path;
        h.compressFile(path);
        break;
    case 2:
        std::cout << "Enter the file path to decompress: ";
        std::cin >> path;
        h.decompressFile(path);
        break;
    case 3:
        std::cout << "Enter the folder path to compress: ";
        std::cin >> path;
        h.compressFolder(path);
        break;
    case 4:
        std::cout << "Enter the folder path to decompress: ";
        std::cin >> path;
        h.decompressFolder(path);
        break;
    case 5:
        std::cout << "Enter the folder path: ";
        std::cin >> path;
        h.info(path);
        break;
    case 6:
        std::cout << "Exiting...\n";
        break;
    default:
        std::cout << "Invalid choice. Please try again.\n";
        break;
    }
}

int main()
{
    huffmanCompress h;
    int choice;
    do
    {
        displayMenu();
        std::cin >> choice;
        handleUserChoice(choice, h);
    } while (choice != 6);

    return 0;
}
