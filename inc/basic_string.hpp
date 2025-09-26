#pragma once

#include <cstring>
#include <cwchar>
#include <iostream>

#include "allocator.hpp"

namespace hf {
template <typename CharType>
struct char_traits {
  typedef CharType char_type;

  static size_t length(const char_type* str) {
    size_t len = 0;
    while (*str++ != char_type()) ++len;
    return len;
  }

  static int compare(const char_type* s1, const char_type* s2, size_t n) {
    for (; n != 0; --n, ++s1, ++s2) {
      if (*s1 < *s2) return -1;
      if (*s1 > *s2) return 1;
    }
    return 0;
  }

  static char_type* copy(char_type* dst, const char_type* src, size_t n) {
    assert(src + n <= dst || dst + n <= src);

    char_type* r = dst;
    while (n--) *dst++ = *src++;
    return r;
  }

  static char_type* move(char_type* dst, const char_type* src, size_t n) {
    char_type* r = dst;

    if (dst < src)
      while (n--) *dst++ = *src++;
    else if (dst > src)
      for (dst += n, src += n; n; --n) *--dst = *--src;

    return r;
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count) {
    char_type* r = dst;
    while (count--) *dst++ = ch;
    return r;
  }
};

template <>
struct char_traits<char> {
  typedef char char_type;

  static size_t length(const char_type* str) noexcept { return std::strlen(str); }

  static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept {
    return std::memcmp(s1, s2, n);
  }

  static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
    assert(src + n <= dst || dst + n <= src);
    return static_cast<char_type*>(std::memcpy(dst, src, n));
  }

  static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept {
    return static_cast<char_type*>(std::memmove(dst, src, n));
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept {
    return static_cast<char_type*>(std::memset(dst, ch, count));
  }
};

template <>
struct char_traits<wchar_t> {
  typedef wchar_t char_type;

  static size_t length(const char_type* str) noexcept { return std::wcslen(str); }

  static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept {
    return std::wmemcmp(s1, s2, n);
  }

  static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
    assert(src + n <= dst || dst + n <= src);
    return static_cast<char_type*>(std::wmemcpy(dst, src, n));
  }

  static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept {
    return static_cast<char_type*>(std::wmemmove(dst, src, n));
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept {
    return static_cast<char_type*>(std::wmemset(dst, ch, count));
  }
};

template <>
struct char_traits<char16_t> {
  typedef char16_t char_type;

  static size_t length(const char_type* str) {
    size_t len = 0;
    while (*str++ != char_type(0)) ++len;
    return len;
  }

  static int compare(const char_type* s1, const char_type* s2, size_t n) {
    for (; n != 0; --n, ++s1, ++s2) {
      if (*s1 < *s2) return -1;
      if (*s1 > *s2) return 1;
    }
    return 0;
  }

  static char_type* copy(char_type* dst, const char_type* src, size_t n) {
    assert(src + n <= dst || dst + n <= src);

    char_type* r = dst;
    while (n--) *dst++ = *src++;
    return r;
  }

  static char_type* move(char_type* dst, const char_type* src, size_t n) {
    char_type* r = dst;

    if (dst < src)
      while (n--) *dst++ = *src++;
    else if (dst > src)
      for (dst += n, src += n; n; --n) *--dst = *--src;

    return r;
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count) {
    char_type* r = dst;
    while (count--) *dst++ = ch;
    return r;
  }
};

template <>
struct char_traits<char32_t> {
  typedef char32_t char_type;

  static size_t length(const char_type* str) {
    size_t len = 0;
    while (*str++ != char_type(0)) ++len;
    return len;
  }

  static int compare(const char_type* s1, const char_type* s2, size_t n) {
    for (; n != 0; --n, ++s1, ++s2) {
      if (*s1 < *s2) return -1;
      if (*s1 > *s2) return 1;
    }
    return 0;
  }

  static char_type* copy(char_type* dst, const char_type* src, size_t n) {
    assert(src + n <= dst || dst + n <= src);

    char_type* r = dst;
    while (n--) *dst++ = *src++;
    return r;
  }

  static char_type* move(char_type* dst, const char_type* src, size_t n) {
    char_type* r = dst;

    if (dst < src)
      while (n--) *dst++ = *src++;
    else if (dst > src)
      for (dst += n, src += n; n; --n) *--dst = *--src;

    return r;
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count) {
    char_type* r = dst;
    while (count--) *dst++ = ch;
    return r;
  }
};

template <typename CharType, typename CharTraits = hf::char_traits<CharType>>
class basic_string {
 public:
  typedef hf::allocator<CharType> Alloc;

  typedef CharTraits char_traits;

  typedef CharType value_type;

  typedef CharType* iterator;
  typedef const CharType* const_iterator;
  typedef CharType* pointer;
  typedef const CharType* const_pointer;
  typedef CharType& reference;
  typedef const CharType& const_reference;

 private:
  iterator _buffer;
  size_t _size;
  size_t _cap;

  static constexpr size_t STRING_INIT_SIZE = 16;

 public:
  basic_string() noexcept { _init(); }

  basic_string(const_pointer str) { init_from(str, 0, char_traits::length(str)); }

  ~basic_string() { destroy_buffer(); }

 public:
  iterator begin() noexcept { return _buffer; }

  iterator end() noexcept { return _buffer + _size; }

  const_iterator begin() const noexcept { return _buffer; }

  const_iterator end() const noexcept { return _buffer + _size; }

  const_iterator cbegin() const noexcept { return _buffer; }

  const_iterator cend() const noexcept { return _buffer + _size; }

  /* ------------------------------------------------------------------------- */

  bool empty() const noexcept { return _size == 0; }

  size_t size() const noexcept { return _size; }

  size_t length() const noexcept { return _size; }

  size_t capacity() const noexcept { return _cap; }

  size_t max_size() const noexcept { return static_cast<size_t>(-1); }

  /* ------------------------------------------------------------------------- */

  reference operator[](size_t n) {
    assert(n <= _size);
    return *(_buffer + n);
  }

  const_reference operator[](size_t n) const {
    assert(n <= _size);
    return *(_buffer + n);
  }

  reference front() noexcept {
    assert(!empty());
    return *begin();
  }

  const_reference front() const noexcept {
    assert(!empty());
    return *begin();
  }

  reference back() noexcept {
    assert(!empty());
    return *(end() - 1);
  }

  const_reference back() const noexcept {
    assert(!empty());
    return *(end() - 1);
  }

  const_pointer data() const noexcept { return _buffer; }

  const_pointer c_str() const noexcept { return _buffer; }

  /* ------------------------------------------------------------------------- */

  basic_string& append(size_t count, value_type ch) noexcept;

  basic_string& append(const_pointer s) noexcept { return append(s, char_traits::length(s)); }

  basic_string& append(const_pointer s, size_t count) noexcept;

  /* ------------------------------------------------------------------------- */

  friend std::ostream& operator<<(std::ostream& os, const basic_string& str) {
    for (size_t i = 0; i < str._size; ++i) os << *(str._buffer + i);
    return os;
  }

 private:
  void _init() noexcept;

  void _init_tail() noexcept { *(_buffer + _size) = value_type(); };

  void init_from(const_pointer src, size_t pos, size_t count) noexcept;

  size_t get_new_cap(size_t add_size) const noexcept;

  void reallocate(size_t n) noexcept;

  void destroy_buffer() noexcept;
};

template <typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>& basic_string<CharType, CharTraits>::append(
    size_t count, value_type ch) noexcept {
  assert(_size <= max_size() - count);

  if (_size + count >= _cap) reallocate(count);

  char_traits::fill(_buffer + _size, ch, count);
  _size += count;
  _init_tail();

  return *this;
}

template <typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>& basic_string<CharType, CharTraits>::append(
    const_pointer s, size_t count) noexcept {
  assert(_size <= max_size() - count);

  if (_size + count >= _cap) reallocate(count);

  char_traits::copy(_buffer + _size, s, count);
  _size += count;
  _init_tail();

  return *this;
}

/* ------------------------------------------------------------------------- */

template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::_init() noexcept {
  _buffer = Alloc::allocate(STRING_INIT_SIZE);
  _size = 0;
  _cap = STRING_INIT_SIZE;
  _init_tail();
}

template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::init_from(const_pointer src, size_t pos,
                                                   size_t count) noexcept {
  auto init_size = std::max(count + 1, STRING_INIT_SIZE);
  _buffer = Alloc::allocate(init_size);
  char_traits::copy(_buffer, src + pos, count);
  _size = count;
  _cap = init_size;
  _init_tail();
}

template <typename CharType, typename CharTraits>
size_t basic_string<CharType, CharTraits>::get_new_cap(size_t add_size) const noexcept {
  auto old_size = capacity();

  if (old_size > max_size() - (old_size >> 1)) {
    return old_size + add_size > max_size() - STRING_INIT_SIZE
               ? old_size + add_size + 1
               : old_size + add_size + STRING_INIT_SIZE;
  }

  return old_size == 0 ? std::max(add_size + 1, STRING_INIT_SIZE)
                       : std::max(old_size + (old_size >> 1), old_size + add_size + 1);
}

template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::reallocate(size_t n) noexcept {
  auto new_cap = get_new_cap(n);
  auto new_buffer = Alloc::allocate(new_cap);
  char_traits::move(new_buffer, _buffer, _size);
  Alloc::deallocate(_buffer);
  _buffer = new_buffer;
  _cap = new_cap;
}

template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::destroy_buffer() noexcept {
  if (_buffer != nullptr) {
    Alloc::deallocate(_buffer, _cap);
    _buffer = nullptr;
    _size = 0;
    _cap = 0;
  }
}

}  // namespace hf
