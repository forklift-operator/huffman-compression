# Huffman Compression

## Introduction
This project implements Huffman coding for file and folder compression in C++.  
It was created as a university project to demonstrate efficient lossless compression using prefix-free binary codes based on character frequency.

## How It Works
The program performs compression and decompression through the following steps:

- **Build Frequency Table**: Reads the input and counts character occurrences.
- **Build Huffman Tree**: Uses a min-heap to build a binary tree based on frequencies.
- **Generate Codes**: Assigns binary codes by traversing the tree.
- **Compress**: Encodes the input into a compressed binary stream, writes metadata and padding information.
- **Decompress**: Reads metadata, rebuilds codes, and decodes the binary stream to restore the original data.

It supports both single files and entire folders.

## Project Structure
- `huffmanCompress.h` / `huffmanCompress.cpp`: Main logic for compressing and decompressing files/folders.
- `minHeap.h` / `minHeap.cpp`: Custom min-heap for building the Huffman tree.
- `.gitignore`: Ignores binaries and build artifacts.

The app offers a simple text menu to select actions like compressing files, decompressing archives, or viewing archive info.

## Usage

### Compile
```
g++ -std=c++17 huffmanCompress.cpp minHeap.cpp -o huffmanCompress
```

### Run
```
./huffmanCompress
```

### You will be prompted to:

- Compress/Decompress a file or folder
- View compressed file info
- Exit

### Compressed files will have a .huff extension, and decompressed outputs are prefixed with huff_.

---

## Acknowledgments
Project developed as part of a university coursework assignment.
Based on the Huffman coding algorithm introduced by David Huffman in 1952.
