#pragma once

#include "allocator.hpp"

namespace hf {

template <typename T>
class vector {
 public:
  typedef T* iterator;
  typedef hf::allocator<T> Alloc;

 private:
  iterator _begin;
  iterator _end;
  iterator _cap;

  static constexpr size_t _INIT_SIZE = 16;

 public:
  vector() noexcept { _init(); };

  explicit vector(size_t n) noexcept { fill_init(n); };

  vector(const vector& rhs) { range_init(rhs._begin, rhs._end); }

  vector(vector&& rhs) noexcept : _begin(rhs._begin), _end(rhs._end), _cap(rhs._cap) {
    rhs._begin = rhs._end = rhs._cap = nullptr;
  }

  vector(std::initializer_list<T> list) { range_init(list.begin(), list.end()); }

  ~vector() {
    destroy_and_recover(_begin, _end);
    _begin = _end = _cap = nullptr;
  }

 public:
  iterator begin() { return _begin; }
  iterator end() { return _end; }

  size_t size() const noexcept { return static_cast<size_t>(_end - _begin); }
  bool empty() const noexcept { return _begin == _end; }

  T& operator[](size_t n) { return *(_begin + n); }
  const T& operator[](size_t n) const { return *(_begin + n); }

  void push_back(const T& value);

  void pop_back();

 public:
  const size_t max_size() const noexcept { return static_cast<size_t>(-1) / sizeof(T); }

  const size_t capacity() const noexcept { return static_cast<size_t>(_cap - _begin); }

 private:
  void _init() noexcept;

  void fill_init(size_t n) noexcept;

  void init_space(size_t len, size_t cap) noexcept;

  template <typename Iter>
  void range_init(Iter first, Iter last);

  const size_t get_new_cap(size_t add_size) const noexcept;

  void reallocate_insert(iterator pos, const T& value) noexcept;

  void destroy_and_recover(iterator first, iterator last);
};

/* ------------------------------------------------------------------------- */

template <typename T>
void vector<T>::push_back(const T& value) {
  if (_cap == nullptr) _init();

  if (_end != _cap) {
    ::new (_end++) T(value);
  } else {
    reallocate_insert(_end, value);
  }
}

template <typename T>
void vector<T>::pop_back() {
  if (empty()) return;
  (_end - 1)->~T();
  _end--;
}

/* ------------------------------------------------------------------------- */

template <typename T>
void vector<T>::_init() noexcept {
  _begin = Alloc::allocate(_INIT_SIZE);
  _end = _begin;
  _cap = _begin + _INIT_SIZE;
}

template <typename T>
void vector<T>::fill_init(size_t n) noexcept {
  size_t init_size = std::max(_INIT_SIZE, n);
  init_space(n, init_size);

  auto cur = _begin;
  while (cur != _end) ::new (cur++) T();
}

template <typename T>
void vector<T>::init_space(size_t len, size_t cap) noexcept {
  _begin = Alloc::allocate(cap);
  _end = _begin + len;
  _cap = _begin + cap;
}

template <typename T>
template <typename Iter>
void vector<T>::range_init(Iter first, Iter last) {
  size_t len = std::distance(first, last);
  size_t init_size = std::max(len, _INIT_SIZE);

  init_space(len, init_size);

  auto cur = _begin;
  while (cur != _end) *cur++ = *first++;
}

template <typename T>
void vector<T>::reallocate_insert(iterator pos, const T& value) noexcept {
  const auto new_size = get_new_cap(1);
  auto new_begin = Alloc::allocate(new_size);

  auto new_end = std::uninitialized_move(_begin, pos, new_begin);
  ::new (new_end++) T(value);
  new_end = std::uninitialized_move(pos, _end, new_end);

  destroy_and_recover(_begin, _end);
  _begin = new_begin;
  _end = new_end;
  _cap = new_begin + new_size;
};

template <typename T>
void vector<T>::destroy_and_recover(iterator first, iterator last) {
  for (auto ptr = first; ptr != last; ++ptr) ptr->~T();
  ::operator delete(first);
}

template <typename T>
const size_t vector<T>::get_new_cap(size_t add_size) const noexcept {
  const auto old_size = capacity();

  if (old_size > max_size() - old_size >> 1) {
    return old_size + add_size > max_size() - _INIT_SIZE ? old_size + add_size
                                                         : old_size + add_size + _INIT_SIZE;
  }

  return old_size == 0 ? std::max(add_size, _INIT_SIZE)
                       : std::max(old_size + (old_size >> 1), old_size + add_size);
};

}  // namespace hf
