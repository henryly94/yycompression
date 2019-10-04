#include "heap.h"
#include "huffman.h"

bool Larger(int a, int b) { return a > b; }

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

  std::vector<int> data = {5, 6, 1, 2, 3, 7, 4, 8};
  yycompression::Heap<int> heap(data, Larger);
  heap.print();
  for (int i = 0; i < 8; i++) {
    std::cout << heap.PopHeap() << std::endl;
    heap.print();
  }
}
