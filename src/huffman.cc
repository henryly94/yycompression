#include "huffman.h"

#include <functional>

namespace yycompression {
namespace huffman {
namespace {

bool NodePtrComparator(const Node *a, const Node *b) {
  return a->freq < b->freq;
}

} // namespace

std::size_t HuffmanUnitHash::operator()(const HuffmanUnit &unit) const {
  std::hash<unsigned int> uint_hash;
  std::hash<int64_t> int64_hash;
  std::size_t hash_value = uint_hash(unit.length);
  for (int i = 0; i < 4; i++) {
    hash_value ^= int64_hash(unit.bits.data[i]);
  }
  return hash_value;
}

bool HuffmanUnitEq::operator()(const HuffmanUnit &a,
                               const HuffmanUnit &b) const {
  return a.bits == b.bits && a.length == b.length;
}

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
  out.write((char *)&unit.length, sizeof(unit.length));
  out.write((char *)&unit.bits.data, 32);
}

std::istream &operator>>(std::istream &in, HuffmanUnit &unit) {
  in.read((char *)&unit.length, sizeof(unit.length));
  in.read((char *)&unit.bits.data, 32);
}

Huffman::Huffman(const std::unordered_map<char, unsigned int> &freq_map) {
  std::vector<Node *> node_ptrs;
  for (const auto &pair : freq_map) {
    Node *node_ptr = factory_.GetNewInstance(pair.first, pair.second);
    node_ptrs.push_back(node_ptr);
  }
  Node *root = buildHuffmanTree(node_ptrs);
  buildMap(root);
  buildReverseMap();
}

Huffman::Huffman(std::ifstream &in, int n) {
  for (int i = 0; i < n; i++) {
    HuffmanUnit unit;
    char ch;
    in.get(ch);
    in >> unit;
    huffman_map_[ch] = std::move(unit);
  }
  buildReverseMap();
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

void Huffman::buildReverseMap() {
  for (const auto &pair : huffman_map_) {
    reverse_map_[pair.second] = pair.first;
  }
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
