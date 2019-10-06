#ifndef BASE_H_
#define BASE_H_

#include <utility>
#include <vector>

namespace yycompression {

template <typename T> class Factory {
public:
  template <typename... Args> T *GetNewInstance(Args &&... args) {
    T *T_ptr = new T{std::forward<Args>(args)...};
    ptrs_.push_back(T_ptr);
    return T_ptr;
  }

  ~Factory() {
    for (T *ptr : ptrs_) {
      delete ptr;
    }
  }

private:
  std::vector<T *> ptrs_;
};

} // namespace yycompression
#endif // BASE_H_
