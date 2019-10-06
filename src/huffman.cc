#include "huffman.h"

namespace yycompression {
namespace huffman {
namespace {

bool NodePtrComparator(const Node *a, const Node *b) {
  return a->freq < b->freq;
}

} // namespace

Bits Bits::operator<<(uint8_t n) {
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

std::ostream &operator<<(std::ostream &out, Bits bits) {
  for (int i = 3; i >= 0; i--) {
    out << std::setfill('0') << std::setw(16) << std::hex << bits.data[i];
  }
}

std::ostream &operator<<(std::ostream &out, const HuffmanUnit &unit) {
  out << unit.length << unit.bits.data[0] << unit.bits.data[1]
      << unit.bits.data[2] << unit.bits.data[3];
}

std::istream &operator>>(std::istream &in, HuffmanUnit &unit) {
  in >> unit.length >> unit.bits.data[0] >> unit.bits.data[1] >>
      unit.bits.data[2] >> unit.bits.data[3];
}

Huffman::Huffman(const std::unordered_map<char, unsigned int> &freq_map) {
  huffman_map_.reserve(256);
  std::vector<Node *> node_ptrs;
  for (const auto &pair : freq_map) {
    Node *node_ptr = factory_.GetNewInstance(pair.first, pair.second);
    node_ptrs.push_back(node_ptr);
  }
  Node *root = buildHuffmanTree(node_ptrs);
  buildMap(root);
}

Node *Huffman::buildHuffmanTree(const std::vector<Node *> &node_ptrs) {
  Heap<Node *> heap{node_ptrs, NodePtrComparator};
  while (heap.size() != 1) {
    Node *a = heap.PopHeap();
    Node *b = heap.PopHeap();
    Node *node_ptr = factory_.GetNewInstance(a, b);
    heap.PushHeap(node_ptr);
  }
  return heap.GetHead();
}

void Huffman::buildMap(Node *root) { buildMap(root, Bits{}, 0); }

void Huffman::buildMap(Node *node, Bits bits, unsigned int length) {
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

} // namespace huffman
} // namespace yycompression
