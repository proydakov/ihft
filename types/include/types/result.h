#pragma once

#include <variant>

namespace ihft::types
{

template<typename T, typename E>
class result final
{
public:
    result(T data) : m_variant(std::move(data))
    {
    }

    result(E error) : m_variant(std::move(error))
    {
    }

    // Returns true if parsing succeeeded.
    bool succeeded() const noexcept
    {
        return std::holds_alternative<T>(m_variant);
    }

    // Returns true if parsing failed.
    bool failed() const noexcept
    {
        return std::holds_alternative<E>(m_variant);
    }

    operator bool() const noexcept
    {
        return succeeded();
    }

    // Get value

    T& value() & noexcept
    {
        return std::get<T>(m_variant);
    }

    const T& value() const & noexcept
    {
        return std::get<T>(m_variant);
    }

    T&& value() && noexcept
    {
        return std::get<T>(std::move(m_variant));
    }

    const T&& value() const && noexcept
    {
        return std::get<T>(std::move(m_variant));
    }

    // Get error

    E& error() & noexcept
    {
        return std::get<E>(m_variant);
    }

    const E& error() const & noexcept
    {
        return std::get<E>(m_variant);
    }

    E&& error() && noexcept
    {
        return std::get<E>(std::move(m_variant));
    }

    const E&& error() const && noexcept
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
