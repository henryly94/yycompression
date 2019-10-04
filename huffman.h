#ifndef HUFFMAN_H_
#define HUFFMAN_H_

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

bool NodePtrComparator(Node *a, Node *b) { return a->freq > b->freq; }

class Huffman {
public:
  std::vector<Node> nodes;

  Node *BuildHuffmanTree() {
    std::vector<Node *> heap;
    for (auto &node : nodes) {
      heap.push_back(&node);
    }
    std::make_heap(heap.begin(), heap.end(), NodePtrComparator);
    while (heap.size() != 1) {
      std::pop_heap(heap.begin(), heap.end(), NodePtrComparator);
      Node *a = heap.back();
      heap.pop_back();
      std::pop_heap(heap.begin(), heap.end(), NodePtrComparator);
      Node *b = heap.back();
      heap.pop_back();
      Node new_node;
      new_node.left = a;
      new_node.right = b;
      new_node.ch = '|';
      new_node.is_leaf = false;
      new_node.freq = a->freq + b->freq;
      nodes.push_back(new_node);
      heap.push_back(&nodes.back());
      std::push_heap(heap.begin(), heap.end(), NodePtrComparator);
    }
    return heap[0];
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
