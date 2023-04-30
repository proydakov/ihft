#pragma once

#include <logger/logger_extra_data.h>

#include <string>
#include <string_view>
#include <type_traits>

namespace ihft::logger
{

//
// This class produces information how to 'represent' origin type.
// And how to 'pack' origin into tuple & extra buffer.
//

// Basic contract

template<typename T>
struct logger_contract
{
    using type = void;
};

// Simple types contracts

#define DECLARE_SIMPLE_LOGGER_CONTRACT(T) \
template<> \
struct logger_contract<T> \
{ \
    using type = T; \
\
    static type pack(logger_extra_data&, T origin) \
    { \
        return origin; \
    } \
}

DECLARE_SIMPLE_LOGGER_CONTRACT(char);
DECLARE_SIMPLE_LOGGER_CONTRACT(signed char);
DECLARE_SIMPLE_LOGGER_CONTRACT(unsigned char);

DECLARE_SIMPLE_LOGGER_CONTRACT(short);
DECLARE_SIMPLE_LOGGER_CONTRACT(unsigned short);
DECLARE_SIMPLE_LOGGER_CONTRACT(int);
DECLARE_SIMPLE_LOGGER_CONTRACT(unsigned int);
DECLARE_SIMPLE_LOGGER_CONTRACT(long);
DECLARE_SIMPLE_LOGGER_CONTRACT(unsigned long);
DECLARE_SIMPLE_LOGGER_CONTRACT(long long);
DECLARE_SIMPLE_LOGGER_CONTRACT(unsigned long long);

DECLARE_SIMPLE_LOGGER_CONTRACT(float);
DECLARE_SIMPLE_LOGGER_CONTRACT(double);
DECLARE_SIMPLE_LOGGER_CONTRACT(long double);

#undef DECLARE_SIMPLE_LOGGER_CONTRACT

//
// String-like types contracts
//
// @todo : support other string types
//
// std::wstring            std::basic_string<wchar_t>  (typedef)
// std::u16string (C++11)  std::basic_string<char16_t> (typedef)
// std::u32string (C++11)  std::basic_string<char32_t> (typedef)
// std::u8string  (C++20)  std::basic_string<char8_t>  (typedef)
//

template<typename T>
concept char_counter = std::is_same_v<T, char>;

template<char_counter T, size_t N>
struct logger_contract<T[N]>
{
    using type = std::string_view;

    static type pack(logger_extra_data& ctx, std::string_view origin)
    {
        return ctx.place(origin);
    }
};

#define DECLARE_STRING_VIEW_LIKE_LOGGER_CONTRACT(T) \
template<> \
struct logger_contract<T> \
{ \
    using type = std::string_view; \
 \
    static type pack(logger_extra_data& ctx, std::string_view origin) \
    { \
        return ctx.place(origin); \
    } \
}

DECLARE_STRING_VIEW_LIKE_LOGGER_CONTRACT(std::string);
DECLARE_STRING_VIEW_LIKE_LOGGER_CONTRACT(std::string_view);

#undef DECLARE_STRING_VIEW_LIKE_LOGGER_CONTRACT

// Pointer types contracts

template<>
struct logger_contract<std::nullptr_t>
{
    using type = const void*;

    static type pack(logger_extra_data&, std::nullptr_t origin)
    {
        return origin;
    }
};

template<typename T>
struct logger_contract<T*>
{
    using type = const void*;

    static type pack(logger_extra_data&, T* origin)
    {
        return origin;
    }
};

template<typename T>
struct logger_contract<const T*>
{
    using type = const void*;

    static type pack(logger_extra_data&, const T* origin)
    {
        return origin;
    }
};

}
