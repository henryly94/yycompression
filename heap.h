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
  explicit Heap(Comparator<T> comparator = std::less<T>{})
      : comparator_(comparator) {}
  explicit Heap(std::vector<T> data, Comparator<T> comparator = std::less<T>{})
      : data_(std::move(data)), comparator_(comparator) {
    make_heap();
  }

  T &GetHead() { return data_[0]; }

  T PopHeap() {
    std::swap(data_.front(), data_.back());
    T ret = std::move(data_.back());
    data_.pop_back();
    top_down(0);
    return ret;
  }

  void PushHeap(const T &t) {
    data_.push_back(t);
    bottom_up();
  }

  unsigned int size() { return data_.size(); }

  bool empty() { return data_.empty(); }

  void print() {
    for (const T &t : data_) {
      std::cout << t << ' ';
    }
    std::cout << std::endl;
  }

private:
  void bottom_up() {
    unsigned int i = data_.size() - 1;
    while (i > 0) {
      if (data_[i] < data_[i / 2]) {
        std::swap(data_[i], data_[i / 2]);
        i /= 2;
      } else {
        return;
      }
    }
  }

  void top_down(unsigned int i) {
    while (i * 2 + 1 < data_.size()) {
      unsigned int left_child = i * 2 + 1;
      unsigned int right_child = left_child + 1;
      unsigned int child = left_child;
      if (right_child < data_.size() &&
          comparator_(data_[right_child], data_[left_child])) {
        child = right_child;
      }
      if (comparator_(data_[child], data_[i])) {
        std::swap(data_[i], data_[child]);
        i = child;
      } else {
        return;
      }
    }
  }

  void make_heap() {
    if (data_.empty()) {
      return;
    }
    for (int i = data_.size() / 2; i >= 0; i--) {
      top_down(i);
    }
  }
  std::vector<T> data_;
  Comparator<T> comparator_;
};

} // namespace yycompression

#endif // HEAP_H_
