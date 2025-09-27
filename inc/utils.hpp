#pragma once

#include <cstddef>

#include "type_traits.hpp"

namespace hf {

template <typename T>
constexpr typename hf::remove_reference<T>::type&& move(T&& t) noexcept {
  return static_cast<typename hf::remove_reference<T>::type&&>(t);
}

/* -------------------------------------------------------------------------------------- */

template <typename T>
constexpr T&& forward(typename hf::remove_reference<T>::type& t) noexcept {
  return static_cast<T&&>(t);
}

template <typename T>
constexpr T&& forward(typename hf::remove_reference<T>::type&& t) noexcept {
  static_assert(!hf::is_lvalue_reference<T>::value,
                "forward must not be used to convert an rvalue to an lvalue");
  return static_cast<T&&>(t);
}

/* -------------------------------------------------------------------------------------- */

template <typename T>
void swap(T& a, T& b) noexcept {
  T tmp = hf::move(a);
  a = hf::move(b);
  b = hf::move(tmp);
}

template <typename T, size_t N>
void swap(T (&a)[N], T (&b)[N]) noexcept {
  for (size_t n = 0; n < N; ++n) hf::swap(a[n], b[n]);
}

}  // namespace hf
