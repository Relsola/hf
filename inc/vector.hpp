#pragma once

#include "allocator.hpp"

namespace hf {

template <typename T>
class vector {
 public:
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

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
    destroy_and_recover(_begin, _end, _cap - _begin);
    _begin = _end = _cap = nullptr;
  }

 public:
  iterator begin() noexcept { return _begin; }

  iterator end() noexcept { return _end; }

  const_iterator begin() const noexcept { return _begin; }

  const_iterator end() const noexcept { return _end; }

  const_iterator cbegin() const noexcept { return _begin; }

  const_iterator cend() const noexcept { return _end; }

  /* ------------------------------------------------------------------------- */

  size_t size() const noexcept { return static_cast<size_t>(_end - _begin); }

  size_t capacity() const noexcept { return static_cast<size_t>(_cap - _begin); }

  size_t max_size() const noexcept { return static_cast<size_t>(-1) / sizeof(T); }

  bool empty() const noexcept { return _begin == _end; }

  /* ------------------------------------------------------------------------- */

  reference operator[](size_t n) noexcept { return *(_begin + n); }

  const_reference operator[](size_t n) const noexcept { return *(_begin + n); }

  reference front() noexcept { return *_begin; }

  const_reference front() const noexcept { return *_begin; }

  reference back() noexcept { return *(_end - 1); }

  const_reference back() const noexcept { return *(_end - 1); }

  pointer data() noexcept { return _begin; }

  const_pointer data() const noexcept { return _begin; }

  /* ------------------------------------------------------------------------- */

  void push_back(const_reference value) noexcept;

  void pop_back() noexcept;

  iterator insert(const_iterator pos, const_reference value) noexcept;

  iterator insert(const_iterator pos, size_t n, const_reference value) noexcept {
    assert(pos >= begin() && pos <= end());
    return fill_insert(const_cast<iterator>(pos), n, value);
  };

  iterator erase(const_iterator pos) noexcept;

  iterator erase(const_iterator first, const_iterator last) noexcept;

  void resize(size_t new_size) noexcept { resize(new_size, T()); };

  void resize(size_t new_size, const_reference value) noexcept;

  void clear() noexcept { erase(begin(), end()); }

  void swap(vector& rhs) noexcept;

 private:
  void _init() noexcept;

  void fill_init(size_t n) noexcept;

  iterator fill_insert(iterator pos, size_t n, const_reference value) noexcept;

  void init_space(size_t len, size_t cap) noexcept;

  template <typename Iter>
  void range_init(Iter first, Iter last) noexcept;

  const size_t get_new_cap(size_t add_size) const noexcept;

  void reallocate_insert(iterator pos, const_reference value) noexcept;

  void destroy_and_recover(iterator first, iterator last, size_t n) noexcept;
};

/* ------------------------------------------------------------------------- */

template <typename T>
void vector<T>::push_back(const_reference value) noexcept {
  if (_end != _cap) {
    Alloc::construct(_end++, value);
  } else {
    reallocate_insert(_end, value);
  }
}

template <typename T>
void vector<T>::pop_back() noexcept {
  if (empty()) return;
  Alloc::destroy(_end - 1);
  _end--;
}

template <typename T>
typename vector<T>::iterator vector<T>::insert(const_iterator pos, const_reference value) noexcept {
  assert(pos >= begin() && pos <= end());

  iterator xpos = _begin + (pos - begin());
  size_t n = pos - _begin;

  if (_end != _cap && xpos == _end) {
    Alloc::construct(_end, value);
    ++_end;
  } else if (_end != _cap) {
    auto copy_value = value;
    std::move(xpos, _end, xpos + 1);
    Alloc::construct(xpos, copy_value);
    ++_end;
  } else {
    reallocate_insert(xpos, value);
  }

  return _begin + n;
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator pos) noexcept {
  assert(pos >= begin() && pos < end());

  iterator xpos = _begin + (pos - begin());
  std::move(xpos + 1, _end, xpos);
  Alloc::destroy(_end-- - 1);
  return xpos;
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) noexcept {
  assert(first >= begin() && last <= end() && first <= last);

  auto n = first - _begin;
  iterator r = _begin + (first - begin());
  Alloc::destroy(std::move(r + (last - first), _end, r), _end);
  _end = _end - (last - first);
  return _begin + n;
}

template <typename T>
void vector<T>::resize(size_t new_size, const_reference value) noexcept {
  auto old_size = size();

  if (new_size < old_size) {
    erase(begin() + new_size, end());
  } else {
    insert(end(), new_size - old_size, value);
  }
}

template <typename T>
void vector<T>::swap(vector<T>& rhs) noexcept {
  if (this != &rhs) {
    std::swap(_begin, rhs._begin);
    std::swap(_end, rhs._end);
    std::swap(_cap, rhs._cap);
  }
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
  std::uninitialized_fill_n(_begin, n, T());
}

template <typename T>
typename vector<T>::iterator vector<T>::fill_insert(iterator pos, size_t n,
                                                    const_reference value) noexcept {
  if (n <= 0) return pos;

  auto x = pos - _begin;
  auto copy_value = value;

  if (static_cast<size_t>(_cap - _end) >= n) {
    auto after_elems = _end - pos;
    auto old_end = _end;

    if (after_elems > n) {
      std::move(_end - n, _end, _end);
      _end += n;
      std::move_backward(pos, old_end - n, old_end);
      std::uninitialized_fill_n(pos, n, copy_value);
    } else {
      _end = std::uninitialized_fill_n(_end, n - after_elems, copy_value);
      _end = std::move(pos, old_end, _end);
      std::uninitialized_fill_n(pos, after_elems, copy_value);
    }
  } else {
    const auto new_size = get_new_cap(n);
    auto new_begin = Alloc::allocate(new_size);
    auto new_end = new_begin;

    new_end = std::uninitialized_move(_begin, pos, new_begin);
    new_end = std::uninitialized_fill_n(new_end, n, copy_value);
    new_end = std::uninitialized_move(pos, _end, new_end);

    Alloc::deallocate(_begin, _cap - _begin);
    _begin = new_begin;
    _end = new_end;
    _cap = _begin + new_size;
  }

  return _begin + x;
}

template <typename T>
void vector<T>::init_space(size_t len, size_t cap) noexcept {
  _begin = Alloc::allocate(cap);
  _end = _begin + len;
  _cap = _begin + cap;
}

template <typename T>
template <typename Iter>
void vector<T>::range_init(Iter first, Iter last) noexcept {
  size_t len = std::distance(first, last);
  size_t init_size = std::max(len, _INIT_SIZE);

  init_space(len, init_size);

  auto cur = _begin;
  while (cur != _end) *cur++ = *first++;
}

template <typename T>
void vector<T>::reallocate_insert(iterator pos, const_reference value) noexcept {
  const auto new_size = get_new_cap(1);
  auto new_begin = Alloc::allocate(new_size);
  auto copy_value = value;
  auto new_end = std::uninitialized_move(_begin, pos, new_begin);
  Alloc::construct(new_end++, copy_value);
  new_end = std::uninitialized_move(pos, _end, new_end);

  destroy_and_recover(_begin, _end, _end - _begin);
  _begin = new_begin;
  _end = new_end;
  _cap = new_begin + new_size;
}

template <typename T>
void vector<T>::destroy_and_recover(iterator first, iterator last, size_t n) noexcept {
  Alloc::destroy(first, last);
  Alloc::deallocate(first);
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
}

}  // namespace hf
