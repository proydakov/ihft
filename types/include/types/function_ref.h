#pragma once

///
/// ----------------------------------------------------------------------------
/// function_ref (Extra additions to <functional>)
/// ----------------------------------------------------------------------------
///
/// This code based on my own experience and some public code:
///
/// https://llvm.org/doxygen/STLFunctionalExtras_8h_source.html
/// https://github.com/rigtorp/Function/blob/master/Function.h
/// https://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible
///
/// An efficient, type-erasing, non-owning reference to a callable. This is
/// intended for use as the type of a function parameter that is not used
/// after the function in question returns.
///
/// Lifetime contract (matches std::function_ref):
///  - The referenced callable must outlive the function_ref.
///  - Passing a temporary callable (e.g. a lambda) directly to a function
///    taking function_ref is safe: the temporary lives until the end of the
///    full-expression, i.e. for the whole call.
///  - Storing a function_ref built from a temporary (for example
///    `function_ref f = [x]{...};`) is NOT safe: the temporary is destroyed
///    at the end of that declaration and later calls read freed memory.
///

#include <cstdint>
#include <memory>
#include <utility>

// Lets Clang warn (-Wdangling, an error under -Werror) when a function_ref is
// initialized from a temporary callable that does not outlive the function_ref,
// while still allowing a temporary to be passed to a function taking function_ref.
#if defined(__clang__)
#define IHFT_FUNCTION_REF_LIFETIMEBOUND [[clang::lifetimebound]]
#else
#define IHFT_FUNCTION_REF_LIFETIMEBOUND
#endif

namespace ihft::types
{

template<typename Fn>
class function_ref;

template<typename Ret, typename ... Params>
class function_ref<Ret(Params...)> final
{
public:
    constexpr function_ref() noexcept = default;
    constexpr function_ref(std::nullptr_t) noexcept
    {
    }

    constexpr function_ref(Ret(*ptr)(Params...)) noexcept
        : m_callable(reinterpret_cast<intptr_t>(ptr))
        , m_callback(callback<Ret(Params...)>)
    {
    }

    template<typename Callable> requires
    // This is not the copy-constructor.
    (!std::is_same_v<std::remove_cvref_t<Callable>, function_ref>) &&
    // Functor must be callable and return a suitable type.
    (std::is_void_v<Ret> || std::is_convertible_v<decltype(std::declval<Callable>()(std::declval<Params>()...)), Ret>)
    constexpr function_ref(Callable&& callable IHFT_FUNCTION_REF_LIFETIMEBOUND) noexcept
        : m_callable(reinterpret_cast<intptr_t>(std::addressof(callable)))
        , m_callback(callback<typename std::remove_reference<Callable>::type>)
    {
    }

    constexpr Ret operator()(Params ... params) const
    {
        return m_callback(m_callable, std::forward<Params>(params) ...);
    }

    constexpr explicit operator bool() const noexcept
    {
        return m_callable != 0;
    }

    friend constexpr void swap(function_ref& f1, function_ref& f2) noexcept
    {
        std::swap(f1.m_callable, f2.m_callable);
        std::swap(f1.m_callback, f2.m_callback);
    }

private:
    using callable_ptr = intptr_t;
    using callback_ptr = Ret(*)(callable_ptr, Params...);

    callable_ptr m_callable = 0;
    callback_ptr m_callback = nullptr;

    template<typename Callable>
    static Ret callback(callable_ptr callable, Params ... params)
    {
        auto ptr = reinterpret_cast<Callable*>(callable);
        return (*ptr)(std::forward<Params>(params) ...);
    }
};

}

#undef IHFT_FUNCTION_REF_LIFETIMEBOUND
