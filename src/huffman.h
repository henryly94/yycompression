#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include "base.h"
#include "heap.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

namespace yycompression {
namespace huffman {

struct Node {
  Node *left = nullptr;
  Node *right = nullptr;
  char ch = '\0';
  int freq = 0;
  Node(char c, unsigned int f)
      : left(nullptr), right(nullptr), ch(c), freq(f) {}
  Node(Node *l, Node *r)
      : left(l), right(r), ch('|'), freq(l->freq + r->freq) {}
};

struct Bits {
  int64_t data[4];
  int64_t get(uint8_t index) { return (data[index / 64] >> (index % 64)) | 1; }

  void set(uint8_t index, int64_t value) {
    data[index / 64] |= (1 & value) << (index % 64);
  }

  Bits operator<<(uint8_t n);

  Bits &operator<<=(uint8_t n) {
    *this = *this << n;
    return *this;
  }
  Bits() : data{0, 0, 0, 0} {}
  Bits(int64_t a, int64_t b, int64_t c, int64_t d) : data{a, b, c, d} {}
  Bits(const Bits &bits)
      : data{bits.data[0], bits.data[1], bits.data[2], bits.data[3]} {}
  Bits &operator=(const Bits &bits) { std::memcpy(data, bits.data, 32); }
  friend std::ostream &operator<<(std::ostream &out, Bits bits);
};

struct HuffmanUnit {
  Bits bits;
  unsigned int length;

  friend std::ostream &operator<<(std::ostream &out, const HuffmanUnit &unit);
  friend std::istream &operator>>(std::istream &in, HuffmanUnit &unit);
};

class Huffman {
public:
  explicit Huffman(const std::unordered_map<char, unsigned int> &freq_map);

  std::unordered_map<char, HuffmanUnit> huffman_map_;

private:
  Node *buildHuffmanTree(const std::vector<Node *> &node_ptrs);

  void buildMap(Node *root);

  void buildMap(Node *node, Bits bits, unsigned int length);

  Factory<Node> factory_;
};

} // namespace huffman
} // namespace yycompression

#endif // HUFFMAN_H_
