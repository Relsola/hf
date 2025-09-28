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

/* -------------------------------------------------------------------------------------- */

template <typename T1, typename T2>
struct pair {
  typedef T1 first_type;
  typedef T2 second_type;

  first_type first;
  second_type second;

  // default constructible
  template <typename U1 = T1, typename U2 = T2,
            typename = typename std::enable_if<std::is_default_constructible<U1>::value &&
                                                   std::is_default_constructible<U2>::value,
                                               void>::type>
  constexpr pair() : first(), second() {}

  // implicit construct for this type
  template <typename U1 = T1, typename U2 = T2,
            typename std::enable_if<std::is_copy_constructible<U1>::value &&
                                        std::is_copy_constructible<U2>::value &&
                                        std::is_convertible<const U1&, T1>::value &&
                                        std::is_convertible<const U2&, T2>::value,
                                    int>::type = 0>
  constexpr pair(const T1& a, const T2& b) : first(a), second(b) {}

  // explicit constructible for this type
  template <typename U1 = T1, typename U2 = T2,
            typename std::enable_if<std::is_copy_constructible<U1>::value &&
                                        std::is_copy_constructible<U2>::value &&
                                        (!std::is_convertible<const U1&, T1>::value ||
                                         !std::is_convertible<const U2&, T2>::value),
                                    int>::type = 0>
  explicit constexpr pair(const T1& a, const T2& b) : first(a), second(b) {}

  pair(const pair& rhs) = default;

  pair(pair&& rhs) = default;

  // implicit constructible for other type
  template <typename U1, typename U2,
            typename std::enable_if<
                std::is_constructible<T1, U1>::value && std::is_constructible<T2, U2>::value &&
                    std::is_convertible<U1&&, T1>::value && std::is_convertible<U2&&, T2>::value,
                int>::type = 0>
  constexpr pair(U1&& a, U2&& b) : first(hf::forward<U1>(a)), second(hf::forward<U2>(b)) {}

  // explicit constructible for other type
  template <typename U1, typename U2,
            typename std::enable_if<
                std::is_constructible<T1, U1>::value && std::is_constructible<T2, U2>::value &&
                    (!std::is_convertible<U1, T1>::value || !std::is_convertible<U2, T2>::value),
                int>::type = 0>
  explicit constexpr pair(U1&& a, U2&& b) : first(hf::forward<U1>(a)), second(hf::forward<U2>(b)) {}

  // implicit constructible for other pair
  template <typename U1, typename U2,
            typename std::enable_if<std::is_constructible<T1, const U1&>::value &&
                                        std::is_constructible<T2, const U2&>::value &&
                                        std::is_convertible<const U1&, T1>::value &&
                                        std::is_convertible<const U2&, T2>::value,
                                    int>::type = 0>
  constexpr pair(const pair<U1, U2>& other) : first(other.first), second(other.second) {}

  // explicit constructible for other pair
  template <typename U1, typename U2,
            typename std::enable_if<std::is_constructible<T1, const U1&>::value &&
                                        std::is_constructible<T2, const U2&>::value &&
                                        (!std::is_convertible<const U1&, T1>::value ||
                                         !std::is_convertible<const U2&, T2>::value),
                                    int>::type = 0>
  explicit constexpr pair(const pair<U1, U2>& other) : first(other.first), second(other.second) {}

  // implicit constructible for other pair
  template <typename U1, typename U2,
            typename std::enable_if<
                std::is_constructible<T1, U1>::value && std::is_constructible<T2, U2>::value &&
                    std::is_convertible<U1, T1>::value && std::is_convertible<U2, T2>::value,
                int>::type = 0>
  constexpr pair(pair<U1, U2>&& other)
      : first(hf::forward<U1>(other.first)), second(hf::forward<U2>(other.second)) {}

  // explicit constructible for other pair
  template <typename U1, typename U2,
            typename std::enable_if<
                std::is_constructible<T1, U1>::value && std::is_constructible<T2, U2>::value &&
                    (!std::is_convertible<U1, T1>::value || !std::is_convertible<U2, T2>::value),
                int>::type = 0>
  explicit constexpr pair(pair<U1, U2>&& other)
      : first(hf::forward<U1>(other.first)), second(hf::forward<U2>(other.second)) {}

  // copy assign for this pair
  pair& operator=(const pair& rhs) {
    if (this != &rhs) {
      first = rhs.first;
      second = rhs.second;
    }
    return *this;
  }

  // move assign for this pair
  pair& operator=(pair&& rhs) {
    if (this != &rhs) {
      first = hf::move(rhs.first);
      second = hf::move(rhs.second);
    }
    return *this;
  }

  // copy assign for other pair
  template <typename U1, typename U2>
  pair& operator=(const pair<U1, U2>& rhs) {
    first = rhs.first;
    second = rhs.second;
    return *this;
  }

  // move assign for other pair
  template <typename U1, typename U2>
  pair& operator=(pair<U1, U2>&& rhs) {
    first = hf::forward<U1>(rhs.first);
    second = hf::forward<U2>(rhs.second);
    return *this;
  }

  ~pair() = default;

  void swap(pair& rhs) {
    if (this != &rhs) {
      hf::swap(first, rhs.first);
      hf::swap(second, rhs.second);
    }
  }
};

template <typename T1, typename T2>
bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <typename T1, typename T2>
bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <typename T1, typename T2>
bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return !(lhs == rhs);
}

template <typename T1, typename T2>
bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return rhs < lhs;
}

template <typename T1, typename T2>
bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return !(rhs < lhs);
}

template <typename T1, typename T2>
bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return !(lhs < rhs);
}

template <typename T1, typename T2>
void swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs) {
  lhs.swap(rhs);
}

template <typename T1, typename T2>
pair<T1, T2> make_pair(T1&& first, T2&& second) {
  return pair<T1, T2>(hf::forward<T1>(first), hf::forward<T2>(second));
}

}  // namespace hf
