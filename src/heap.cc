#include "heap.h"
#include "huffman.h"

namespace yycompression {

template <typename T>
Heap<T>::Heap(Comparator<T> comparator) : comparator_(comparator) {}

template <typename T>
Heap<T>::Heap(std::vector<T> data, Comparator<T> comparator)
    : data_(std::move(data)), comparator_(comparator) {
  make_heap();
}

template <typename T> T Heap<T>::PopHeap() {
  std::swap(data_.front(), data_.back());
  T ret = data_.back();
  data_.pop_back();
  top_down(0);
  return ret;
}

template <typename T> void Heap<T>::PushHeap(const T &t) {
  data_.push_back(t);
  bottom_up();
}

template <typename T> void Heap<T>::bottom_up() {
  unsigned int i = data_.size() - 1, father;
  while (i > 0) {
    father = (i - 1) / 2;
    if (comparator_(data_[i], data_[father])) {
      std::swap(data_[i], data_[father]);
      i = father;
    } else {
      return;
    }
  }
}

template <typename T> void Heap<T>::top_down(unsigned int i) {
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

template <typename T> void Heap<T>::make_heap() {
  if (data_.empty()) {
    return;
  }
  for (int i = data_.size() / 2; i >= 0; i--) {
    top_down(i);
  }
}

template class Heap<huffman::Node *>;

} // namespace yycompression
