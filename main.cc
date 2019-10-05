#include "heap.h"
#include "huffman.h"

void print_help() {
  std::cout << "Usage: " << std::endl;
  std::cout << "  ./main src_file dst_file" << std::endl;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    print_help();
  }
  yycompression::huffman::Encode(argv[1], argv[2]);
  return 0;
}
