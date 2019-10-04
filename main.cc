#include "heap.h"
#include "huffman.h"

bool Larger(int a, int b) { return a > b; }

int main() {
  yycompression::huffman::Huffman huffman({
      {'a', 5},
      {'b', 10},
      {'c', 2},
      {'d', 4},
      {'e', 25},
  });

  std::vector<int> data = {5, 6, 1, 2, 3, 7, 4, 8};
  yycompression::Heap<int> heap(data, Larger);
  heap.print();
  for (int i = 0; i < 8; i++) {
    std::cout << heap.PopHeap() << std::endl;
    heap.print();
  }
}
