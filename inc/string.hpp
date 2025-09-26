#pragma once

#include "basic_string.hpp"

namespace hf {

template <class _CharT>
struct char_traits;

template <>
struct char_traits<char>;

template <>
struct char_traits<wchar_t>;

template <>
struct char_traits<char16_t>;

template <>
struct char_traits<char32_t>;

template <typename CharType, typename CharTraits>
class basic_string;

typedef basic_string<char> string;

typedef basic_string<wchar_t> wstring;

typedef basic_string<char16_t> u16string;

typedef basic_string<char32_t> u32string;

}  // namespace hf
