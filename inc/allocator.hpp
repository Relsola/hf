#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>

namespace hf {

template <typename T>
class allocator {
 public:
  static T* allocate() noexcept;
  static T* allocate(size_t n) noexcept;

  static void deallocate(T* ptr) noexcept;
  static void deallocate(T* ptr, size_t n) noexcept;

  static void construct(T* ptr) noexcept;
  static void construct(T* ptr, const T& value) noexcept;
  static void construct(T* ptr, T&& value) noexcept;

  static void destroy(T* ptr) noexcept;
  static void destroy(T* first, T* last) noexcept;
};

template <typename T>
T* allocator<T>::allocate() noexcept {
  return static_cast<T*>(::operator new(sizeof(T)));
}

template <typename T>
T* allocator<T>::allocate(size_t n) noexcept {
  return n <= 0 ? nullptr : static_cast<T*>(::operator new(n * sizeof(T)));
}

template <typename T>
void allocator<T>::deallocate(T* ptr) noexcept {
  if (ptr != nullptr) ::operator delete(ptr);
}

template <typename T>
void allocator<T>::deallocate(T* ptr, size_t) noexcept {
  if (ptr != nullptr) ::operator delete(ptr);
}

template <typename T>
void allocator<T>::construct(T* ptr) noexcept {
  ::new ((void*)ptr) T();
};

template <typename T>
void allocator<T>::construct(T* ptr, const T& value) noexcept {
  ::new ((void*)ptr) T(value);
};

template <typename T>
void allocator<T>::construct(T* ptr, T&& value) noexcept {
  ::new ((void*)ptr) T(std::move(value));
};

template <typename T>
void allocator<T>::destroy(T* ptr) noexcept {
  if (ptr != nullptr) ptr->~T();
}

template <typename T>
void allocator<T>::destroy(T* first, T* last) noexcept {
  while (first != last) destroy(first++);
}

}  // namespace hf
