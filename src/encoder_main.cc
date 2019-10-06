#include "huffman.h"

#include <fstream>
#include <unordered_map>

namespace yycompression {
namespace huffman {
namespace {

void print_bin(char ch) {
  std::cout << "bin:";
  for (int i = 0; i < 8; i++) {
    if ((ch >> (7 - i)) & 1) {
      std::cout << '1';
    } else {
      std::cout << '0';
    }
  }
  std::cout << std::endl;
}

} // namespace
bool Encode(const char *in_filename, const char *out_filename) {
  std::ifstream in(in_filename, std::ifstream::in | std::ifstream::binary);
  std::ofstream out(out_filename, std::ofstream::out | std::ofstream::binary);
  char ch;
  std::unordered_map<char, unsigned int> freq_map;
  int byte_count = 0;
  while (in.get(ch)) {
    if (freq_map.find(ch) == freq_map.end()) {
      freq_map[ch] = 0;
    }
    freq_map[ch]++;
    byte_count++;
  }
  Huffman huffman(freq_map);
  out << huffman.huffman_map_.size();
  for (const auto &pair : huffman.huffman_map_) {
    out.put(pair.first);
    out << pair.second;
  }
  out << byte_count;

  std::ifstream in_again(in_filename,
                         std::ifstream::in | std::ifstream::binary);
  unsigned int next_index = 0;
  char orig_buf;
  char encoded_buf = 0;
  while (in_again.get(orig_buf)) {
    auto &unit = huffman.huffman_map_[orig_buf];
    int index = unit.length - 1;
    while (index >= 0) {
      int64_t bit = unit.bits.get(index);
      encoded_buf |= bit << (7 - next_index);
      next_index++;
      if (next_index == 8) {
        out.put(encoded_buf);
        next_index = 0;
        encoded_buf = 0;
      }
      --index;
    }
  }
  if (next_index != 0) {
    out.put(encoded_buf);
  }
  return true;
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
