#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include "heap.h"
#include <algorithm>
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
  bool is_leaf = true;
  unsigned int freq = 0;
  Node(char c, unsigned int f)
      : left(nullptr), right(nullptr), ch(c), is_leaf(true), freq(f) {}
  Node(Node *l, Node *r)
      : left(l), right(r), ch('|'), is_leaf(false), freq(l->freq + r->freq) {}
};

bool NodePtrComparator(const Node *a, const Node *b) {
  return a->freq < b->freq;
}

class Huffman {
public:
  struct HuffmanUnit {
    char ch;
    unsigned short int length;
  };

  explicit Huffman(std::unordered_map<char, unsigned int> freq_map) {
    std::vector<Node> nodes;
    nodes.reserve(freq_map.size());
    for (const auto &pair : freq_map) {
      nodes.emplace_back(pair.first, pair.second);
    }
    Node *root = buildHuffmanTree(nodes);
    buildMap(root);
    print(root);
    for (const auto &pair : huffman_map_) {
      std::cout << pair.first << ' ';
      printBinary(pair.second);
      std::cout << std::endl;
    }
  }

  Node *buildHuffmanTree(std::vector<Node> &nodes) {
    std::vector<Node *> node_ptrs;
    for (auto &node : nodes) {
      node_ptrs.push_back(&node);
    }
    Heap<Node *> heap{std::move(node_ptrs), NodePtrComparator};
    while (heap.size() != 1) {
      Node *a = heap.PopHeap();
      Node *b = heap.PopHeap();
      nodes.emplace_back(a, b);
      heap.PushHeap(&nodes.back());
    }
    return heap.GetHead();
  }

  void buildMap(Node *root) { buildMap(root, 0x00, 0); }

  void buildMap(Node *node, char ch, unsigned short int length) {
    if (node == nullptr)
      return;
    if (node->is_leaf) {
      huffman_map_[node->ch] = {ch, length};
    } else {
      char left_ch = ch << 1;
      buildMap(node->left, left_ch, length + 1);
      char right_ch = left_ch | 0x01;
      buildMap(node->right, right_ch, length + 1);
    }
  }

  void printBinary(HuffmanUnit unit) {
    for (int i = 7 - (8 - unit.length); i >= 0; --i) {
      std::cout << ((unit.ch & (1 << i)) ? '1' : '0');
    }
  }

  void print(const Node *node) {
    if (node != nullptr) {
      std::cout << node->ch << ' ' << node->freq << std::endl;
      print(node->left);
      print(node->right);
    }
  }

  std::unordered_map<char, HuffmanUnit> huffman_map_;
};

} // namespace huffman
} // namespace yycompression

#endif // HUFFMAN_H_
