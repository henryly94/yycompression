#include "huffman.h"

int main() {
  yycompression::huffman::Huffman huffman;
  std::vector<yycompression::huffman::Node> nodes = {
      {nullptr, nullptr, 'a', true, 5},  {nullptr, nullptr, 'b', true, 10},
      {nullptr, nullptr, 'c', true, 2},  {nullptr, nullptr, 'd', true, 4},
      {nullptr, nullptr, 'e', true, 25},
  };
  huffman.nodes = nodes;
  auto *node_ptr = huffman.BuildHuffmanTree();
  huffman.print(node_ptr);
}
