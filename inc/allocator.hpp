#pragma once

#include <cstddef>
#include <memory>

namespace hf {

template <typename T>
class allocator {
 public:
  static T* allocate();
  static T* allocate(size_t n);
};

template <typename T>
T* allocator<T>::allocate() {
  return static_cast<T*>(::operator new(sizeof(T)));
}

template <typename T>
T* allocator<T>::allocate(size_t n) {
  return n <= 0 ? nullptr : static_cast<T*>(::operator new(n * sizeof(T)));
}

}  // namespace hf
