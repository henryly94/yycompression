#ifndef HUFFMAN_H_
#define HUFFMAN_H_

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

#define MAX_ENCODING_LENGTH 8

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

bool NodePtrComparator(const Node *a, const Node *b) {
  return a->freq < b->freq;
}

class NodeFactory {
public:
  Node *GetNode(Node *l, Node *r) {
    Node *node_ptr = new Node(l, r);
    node_ptrs_.push_back(node_ptr);
    return node_ptr;
  }

  Node *GetNode(char c, unsigned int f) {
    Node *node_ptr = new Node(c, f);
    node_ptrs_.push_back(node_ptr);
    return node_ptr;
  }

  ~NodeFactory() {
    for (Node *node_ptr : node_ptrs_) {
      delete node_ptr;
    }
  }

private:
  std::vector<Node *> node_ptrs_;
};

struct Bits {
  int64_t data[4];
  int64_t get(uint8_t index) { return (data[index / 64] >> (index % 64)) | 1; }

  void set(uint8_t index, int64_t value) {
    data[index / 64] |= (1 & value) << (index % 64);
  }

  Bits operator<<(uint8_t n) {
    if (n < 64) {
      return Bits{data[0] << n, data[1] << n | data[0] >> (64 - n),
                  data[2] << n | data[1] >> (64 - n),
                  data[3] << n | data[2] >> (64 - n)};
    } else if (n < 128) {
      return Bits{0, data[0] << (n - 64),
                  data[1] << (n - 64) | data[0] >> (128 - n),
                  data[2] << (n - 64) | data[1] >> (128 - n)};
    } else if (n < 192) {
      return Bits{0, 0, data[0] << (n - 128),
                  data[1] << (n - 128) | data[0] >> (192 - n)};
    } else {
      return Bits{0, 0, 0, data[0] << (n - 192)};
    }
  }

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

inline std::ostream &operator<<(std::ostream &out, Bits bits) {
  for (int i = 3; i >= 0; i--) {
    out << std::setfill('0') << std::setw(16) << std::hex << bits.data[i];
  }
}

struct HuffmanUnit {
  Bits bits;
  unsigned int length;

  friend std::ostream &operator<<(std::ostream &out, const HuffmanUnit &unit);
  friend std::istream &operator>>(std::istream &in, HuffmanUnit &unit);
};

inline std::ostream &operator<<(std::ostream &out, const HuffmanUnit &unit) {
  out << unit.length << unit.bits.data[0] << unit.bits.data[1]
      << unit.bits.data[2] << unit.bits.data[3];
}

inline std::istream &operator>>(std::istream &in, HuffmanUnit &unit) {
  in >> unit.length >> unit.bits.data[0] >> unit.bits.data[1] >>
      unit.bits.data[2] >> unit.bits.data[3];
}

class Huffman {
public:
  explicit Huffman(std::unordered_map<char, unsigned int> freq_map) {
    huffman_map_.reserve(256);
    std::vector<Node *> node_ptrs;
    for (const auto &pair : freq_map) {
      Node *node_ptr = factory_.GetNode(pair.first, pair.second);
      node_ptrs.push_back(node_ptr);
    }
    Node *root = buildHuffmanTree(&node_ptrs);
    buildMap(root);
  }

  std::unordered_map<char, HuffmanUnit> huffman_map_;

private:
  Node *buildHuffmanTree(std::vector<Node *> *node_ptrs) {
    Heap<Node *> heap{*node_ptrs, NodePtrComparator};
    while (heap.size() != 1) {
      Node *a = heap.PopHeap();
      Node *b = heap.PopHeap();
      Node *node_ptr = factory_.GetNode(a, b);
      heap.PushHeap(node_ptr);
    }
    return heap.GetHead();
  }
  void buildMap(Node *root) { buildMap(root, Bits{}, 0); }

  void buildMap(Node *node, Bits bits, unsigned int length) {
    if (node == nullptr)
      return;
    if (node->left == nullptr && node->right == nullptr) {
      huffman_map_[node->ch] = {bits, length};
    } else {
      bits <<= 1;
      buildMap(node->left, bits, length + 1);
      bits.set(0, 1);
      buildMap(node->right, bits, length + 1);
    }
  }

  NodeFactory factory_;
};

bool Encode(const char *in_filename, const char *out_filename) {
  std::ifstream in(in_filename, std::ifstream::in | std::ifstream::binary);
  std::ofstream out(out_filename, std::ofstream::out | std::ofstream::binary);
  char ch = '0';
  std::unordered_map<char, unsigned int> freq_map;
  while (in.get(ch)) {
    if (freq_map.find(ch) == freq_map.end()) {
      freq_map[ch] = 0;
    }
    freq_map[ch] += 1;
  }
  Huffman huffman(freq_map);
  for (const auto &pair : huffman.huffman_map_) {
    out << pair.first << pair.second;
  }
  out << '\n' << '\n';
  std::ifstream in_again(in_filename,
                         std::ifstream::in | std::ifstream::binary);
  unsigned int next_index = 0;
  char orig_buf;
  int64_t encoded_buf = 0;
  while (in_again.get(orig_buf)) {
    auto &unit = huffman.huffman_map_[ch];
    int len = unit.length - 1;
    while (len >= 0) {
      int64_t bit = unit.bits.get(len);
      encoded_buf |= bit << (63 - next_index);
      next_index++;
      if (next_index == 64) {
        out << encoded_buf;
        next_index = 0;
        encoded_buf = 0;
      }
      --len;
    }
  }
  if (next_index != 0) {
    out << encoded_buf;
  }
}

} // namespace huffman
} // namespace yycompression

#endif // HUFFMAN_H_
