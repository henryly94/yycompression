#include "huffman.h"

#include <fstream>

namespace yycompression {
namespace huffman {

bool Decode(const char *in_filename, const char *out_filename) {
  std::ifstream in(in_filename, std::ifstream::in | std::ifstream::binary);
  std::ofstream out(out_filename, std::ofstream::out | std::ofstream::binary);
  int n;
  in >> n;
  Huffman huffman(in, n);

  int byte_count;
  in >> byte_count;
  char ch;
  HuffmanUnit unit;
  while (in.get(ch)) {
    for (int i = 7; i >= 0; --i) {
      unit.bits <<= 1;
      unit.bits.set(0, (ch >> i) & 1);
      unit.length += 1;
      auto it = huffman.reverse_map_.find(unit);
      if (it != huffman.reverse_map_.end() && byte_count > 0) {
        out.put(it->second);
        unit = HuffmanUnit{};
        byte_count--;
      }
    }
  }
}

} // namespace huffman
} // namespace yycompression

void print_help() {
  std::cout << "Usage: " << std::endl;
  std::cout << "  ./main src_file dst_file" << std::endl;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    print_help();
  }
  yycompression::huffman::Decode(argv[1], argv[2]);
  return 0;
}
