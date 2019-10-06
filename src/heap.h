#ifndef HEAP_H_
#define HEAP_H_

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

namespace yycompression {

template <typename T>
using Comparator = std::function<bool(const T &, const T &)>;

template <typename T> class Heap {
public:
  explicit Heap(Comparator<T> comparator = std::less<T>{});

  explicit Heap(std::vector<T> data, Comparator<T> comparator = std::less<T>{});

  T GetHead() { return data_[0]; }

  T PopHeap();

  void PushHeap(const T &t);

  unsigned int size() { return data_.size(); }

  bool empty() { return data_.empty(); }

  void print() {
    for (const T &t : data_) {
      std::cout << t << ' ';
    }
    std::cout << std::endl;
  }

private:
  void bottom_up();

  void top_down(unsigned int i);

  void make_heap();

  std::vector<T> data_;
  Comparator<T> comparator_;
};

} // namespace yycompression

#endif // HEAP_H_
