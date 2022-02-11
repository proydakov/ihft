#pragma once

#include <variant>

//
// This code was inspired by:
//
// https://doc.rust-lang.org/std/result/enum.Result.html
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0323r10.html
//
// Class template result<T, E> is a vocabulary type which contains an expected value of type T, or an error E.
//

namespace ihft::types
{

template<typename T, typename E>
class result final
{
public:
    constexpr result(T data) : m_variant(std::move(data))
    {
    }

    constexpr result(E error) : m_variant(std::move(error))
    {
    }

    constexpr bool has_value() const noexcept
    {
        return std::holds_alternative<T>(m_variant);
    }

    // Returns true if parsing succeeeded.
    constexpr bool succeeded() const noexcept
    {
        return has_value();
    }

    // Returns true if parsing failed.
    constexpr bool failed() const noexcept
    {
        return std::holds_alternative<E>(m_variant);
    }

    constexpr operator bool() const noexcept
    {
        return has_value();
    }

    // Get value

    constexpr T& value() & noexcept
    {
        return std::get<T>(m_variant);
    }

    constexpr const T& value() const & noexcept
    {
        return std::get<T>(m_variant);
    }

    constexpr T&& value() && noexcept
    {
        return std::get<T>(std::move(m_variant));
    }

    constexpr const T&& value() const && noexcept
    {
        return std::get<T>(std::move(m_variant));
    }

    // Get error

    constexpr E& error() & noexcept
    {
        return std::get<E>(m_variant);
    }

    constexpr const E& error() const & noexcept
    {
        return std::get<E>(m_variant);
    }

    constexpr E&& error() && noexcept
    {
        return std::get<E>(std::move(m_variant));
    }

    constexpr const E&& error() const && noexcept
    {
        return std::get<E>(std::move(m_variant));
    }

    template<typename S>
    friend S& operator<<(S& os, const result<T, E>& iresult)
    {
        if (iresult)
        {
            os << iresult.value();
        }
        else
        {
            os << iresult.error();
        }
        return os;
    }

private:
    std::variant<T, E> m_variant;
};

}
