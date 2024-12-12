# Huffman-Compress-Decompress

## Overview
Huffman-Compress-Decompress is a C++ application for file compression and decompression using the Huffman coding algorithm.

## Features
- **File Compression**: Converts input data into a compressed binary format using Huffman codes.
- **File Decompression**: Restores the original data from the compressed binary format.
- **Visual Output**: Displays Huffman tree, character frequencies, and codes.

## How It Works
### Compression
1. Reads the input file and calculates character frequencies.
2. Constructs a Huffman tree based on the frequencies.
3. Generates Huffman codes for each character.
4. Encodes the input file into a binary format and saves it.
5. Saves the character frequencies for later decompression.

### Decompression
1. Reads the character frequency file to rebuild the Huffman tree.
2. Decodes the binary data back into the original file content.

## File Format
- **Compressed Data File**: `compressed_data.txt` (contains the size of the compressed data).
- **Character Frequency File**: `char_freq.txt` (contains character frequencies for rebuilding the Huffman tree).
- **Output File**: `PROJECT2.dsaproj` (stores the compressed binary data).

## Known Limitations
- Designed for text-based files. Behavior for binary files may vary.
- Uses fixed-width encoding (6 bits) for binary output.
