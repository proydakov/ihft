#pragma once

///
/// ----------------------------------------------------------------------------
/// scope_exit
/// ----------------------------------------------------------------------------
///
/// This code based on my own experience and some public code:
///
/// https://llvm.org/docs/doxygen/ScopeExit_8h_source.html
///
/// This class allows to use lambda as scope destructor.
///

#include <type_traits>
#include <utility>

namespace ihft::types
{

template <typename Callable>
class scope_exit final
{
public:
    template <typename Fp>
    explicit scope_exit(Fp &&F)
        : m_exit_function(std::forward<Fp>(F))
        , m_engaged(true)
    {
    }

    scope_exit(scope_exit &&other) noexcept
        : m_exit_function(std::move(other.m_exit_function))
        , m_engaged(other.m_engaged)
    {
        other.release();
    }

    scope_exit(const scope_exit &) = delete;
    scope_exit &operator=(scope_exit &&) = delete;
    scope_exit &operator=(const scope_exit &) = delete;

    void release()
    {
        m_engaged = false;
    }

    ~scope_exit()
    {
        if (m_engaged) {
            m_exit_function();
        }
    }

private:
    Callable m_exit_function;
    bool m_engaged; // False once moved-from or release().
};

template <typename Callable>
[[nodiscard]] scope_exit<std::decay_t<Callable>> make_scope_exit(Callable &&F)
{
    return scope_exit<std::decay_t<Callable>>(std::forward<Callable>(F));
}

}
