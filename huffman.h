#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include "heap.h"
#include <algorithm>
#include <iostream>
#include <vector>

namespace yycompression {
namespace huffman {

struct Node {
  Node *left = nullptr;
  Node *right = nullptr;
  char ch = '\0';
  bool is_leaf = true;
  unsigned int freq = 0;
};

bool NodePtrComparator(const Node *a, const Node *b) {
  return a->freq < b->freq;
}

class Huffman {
public:
  std::vector<Node> nodes;

  Node *BuildHuffmanTree() {
    std::vector<Node *> node_ptrs;
    for (auto &node : nodes) {
      node_ptrs.push_back(&node);
    }
    Heap<Node *> heap{std::move(node_ptrs), NodePtrComparator};
    while (heap.size() != 1) {
      Node *a = heap.PopHeap();
      Node *b = heap.PopHeap();
      Node new_node;
      new_node.left = a;
      new_node.right = b;
      new_node.ch = '|';
      new_node.is_leaf = false;
      new_node.freq = a->freq + b->freq;
      nodes.push_back(new_node);
      heap.PushHeap(&nodes.back());
    }
    return heap.GetHead();
  }

  void print(const Node *node) {
    if (node != nullptr) {
      std::cout << node->ch << ' ' << node->freq << std::endl;
      print(node->left);
      print(node->right);
    }
  }
};

} // namespace huffman
} // namespace yycompression

#endif // HUFFMAN_H_
