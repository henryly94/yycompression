#include "huffman.h"

#include <fstream>
#include <unordered_map>

namespace yycompression {
namespace huffman {

bool Encode(const char *in_filename, const char *out_filename) {
  std::ifstream in(in_filename, std::ifstream::in | std::ifstream::binary);
  std::ofstream out(out_filename, std::ofstream::out | std::ofstream::binary);
  char ch = '0';
  std::unordered_map<char, unsigned int> freq_map;
  while (in.get(ch)) {
    if (freq_map.find(ch) == freq_map.end()) {
      freq_map[ch] = 0;
    }
    freq_map[ch] += 1;
  }
  Huffman huffman(freq_map);
  for (const auto &pair : huffman.huffman_map_) {
    out << pair.first << pair.second;
  }
  out << '\n' << '\n';
  std::ifstream in_again(in_filename,
                         std::ifstream::in | std::ifstream::binary);
  unsigned int next_index = 0;
  char orig_buf;
  int64_t encoded_buf = 0;
  while (in_again.get(orig_buf)) {
    auto &unit = huffman.huffman_map_[ch];
    int len = unit.length - 1;
    while (len >= 0) {
      int64_t bit = unit.bits.get(len);
      encoded_buf |= bit << (63 - next_index);
      next_index++;
      if (next_index == 64) {
        out << encoded_buf;
        next_index = 0;
        encoded_buf = 0;
      }
      --len;
    }
  }
  if (next_index != 0) {
    out << encoded_buf;
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
  yycompression::huffman::Encode(argv[1], argv[2]);
  return 0;
}
