#pragma once

///
/// ----------------------------------------------------------------------------
/// function_ref (Extra additions to <functional>)
/// ----------------------------------------------------------------------------
///
/// This code based of my experience and some LLVM code:
/// https://llvm.org/doxygen/STLFunctionalExtras_8h_source.html
///
/// An efficient, type-erasing, non-owning reference to a callable. This is
/// intended for use as the type of a function parameter that is not used
/// after the function in question returns.
///
/// This class does not own the callable, so it is not in general safe to store
/// a function_ref.
///

#include <utility>

namespace ihft::types
{

template<typename T>
class function_ref;

template<typename RETURN, typename ... INPUTS>
class function_ref<RETURN(INPUTS ...)> final
{
public:
    //
    // static methods. They allow the client to create a function_ref.
    //
    template<RETURN(*func)(INPUTS...)>
    constexpr static function_ref function() noexcept
    {
        return function_ref(&callback_function<func>);
    }

    template<class T, RETURN(T::*meth)(INPUTS...)>
    constexpr static function_ref method(T& obj) noexcept
    {
        return function_ref(&obj, &callback_method<T, meth>);
    }

    template<class T, RETURN(T::*meth)(INPUTS...) const>
    constexpr static function_ref const_method(T& obj) noexcept
    {
        return function_ref(&obj, &callback_const_method<T, meth>);
    }

    template<class T, RETURN(T::*meth)(INPUTS...) const>
    constexpr static function_ref const_method(T const& obj) noexcept
    {
        return function_ref(&obj, &callback_const_method<T, meth>);
    }

    template<class T>
    constexpr static function_ref functor(T& obj) noexcept
    {
        return function_ref(&obj, &callback_method<T, &T::operator()>);
    }

    template<class T>
    constexpr static function_ref const_functor(T& obj) noexcept
    {
        return function_ref(&obj, &callback_const_method<T, &T::operator()>);
    }

    template<class T>
    constexpr static function_ref const_functor(T const& obj) noexcept
    {
        return function_ref(&obj, &callback_const_method<T, &T::operator()>);
    }

    //
    // normal methods.
    //
    constexpr function_ref() noexcept = default;
    constexpr function_ref(std::nullptr_t) noexcept
    {
    }

    constexpr RETURN operator()(INPUTS ... params) const
    {
        return (*m_func)(m_obj, std::forward<INPUTS>(params) ...);
    }

    constexpr explicit operator bool() const noexcept
    {
        return m_func != nullptr;
    }

private:
    using CallablePtr = void*;
    using CallableConstPtr = void const*;
    using FunctionPtr = RETURN(*)(CallablePtr, INPUTS...);

    CallablePtr m_obj = nullptr;
    FunctionPtr m_func = nullptr;

    constexpr function_ref(FunctionPtr func) noexcept
        : m_obj(nullptr)
        , m_func(func)
    {
    }

    constexpr function_ref(CallablePtr obj, FunctionPtr func) noexcept
        : m_obj(obj)
        , m_func(func)
    {
    }

    constexpr function_ref(CallableConstPtr obj, FunctionPtr func) noexcept
        : m_obj(const_cast<CallablePtr>(obj))
        , m_func(func)
    {
    }

    template<RETURN(*function)(INPUTS...)>
    constexpr static RETURN callback_function(CallablePtr, INPUTS... params)
    {
        return (*function)(std::forward<INPUTS>(params)...);
    }

    template<typename T, RETURN(T::*method)(INPUTS...)>
    constexpr static RETURN callback_method(CallablePtr obj, INPUTS... params)
    {
        auto p = reinterpret_cast<T*>(obj);
        return (p->*method)(std::forward<INPUTS>(params)...);
    }

    template<typename T, RETURN(T::*method)(INPUTS...) const>
    constexpr static RETURN callback_const_method(CallablePtr obj, INPUTS... params)
    {
        auto p = reinterpret_cast<const T*>(obj);
        return (p->*method)(std::forward<INPUTS>(params)...);
    }
};

}
