#pragma once

#include <type_traits>

namespace hf {

template <typename T>
struct remove_reference {
  typedef T type;
};

template <typename T>
struct remove_reference<T&> {
  typedef T type;
};

template <typename T>
struct remove_reference<T&&> {
  typedef T type;
};

/* ------------------------------------------------------------------------- */

template <typename T, T v>
struct integral_constant {
  static constexpr T value = v;
};

using true_type = integral_constant<bool, true>;

using false_type = integral_constant<bool, false>;

/* ------------------------------------------------------------------------- */

template <typename>
struct is_lvalue_reference : public false_type {};

template <typename T>
struct is_lvalue_reference<T&> : public true_type {};

/* ------------------------------------------------------------------------- */

template <typename T1, typename T2>
struct pair;

template <typename T>
struct is_pair : hf::false_type {};

template <typename T1, typename T2>
struct is_pair<hf::pair<T1, T2>> : hf::true_type {};

}  // namespace hf
