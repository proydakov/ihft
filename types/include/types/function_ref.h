#pragma once

///
/// ----------------------------------------------------------------------------
/// function_ref (Extra additions to <functional>)
/// ----------------------------------------------------------------------------
///
/// This code based of my experience and some code:
/// https://llvm.org/doxygen/STLFunctionalExtras_8h_source.html
/// https://github.com/rigtorp/Function/blob/master/Function.h
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

template<typename Fn>
class function_ref;

template<typename Ret, typename ... Params>
class function_ref<Ret(Params ...)> final
{
public:
    //
    // static methods. They allow the client to create a function_ref.
    //
    template<Ret(*func)(Params...)>
    constexpr static function_ref function() noexcept
    {
        return function_ref(&callback_function<func>);
    }

    template<class T, Ret(T::*meth)(Params...)>
    constexpr static function_ref method(T& obj) noexcept
    {
        return function_ref(&obj, &callback_method<T, meth>);
    }

    template<class T, Ret(T::*meth)(Params...) const>
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

    constexpr Ret operator()(Params ... params) const
    {
        return (*m_func)(m_obj, std::forward<Params>(params) ...);
    }

    constexpr explicit operator bool() const noexcept
    {
        return m_func != nullptr;
    }

private:
    using CallablePtr = void*;
    using CallableConstPtr = void const*;
    using FunctionPtr = Ret(*)(CallablePtr, Params...);

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

    template<Ret(*function)(Params...)>
    constexpr static Ret callback_function(CallablePtr, Params... params)
    {
        return (*function)(std::forward<Params>(params)...);
    }

    template<typename T, Ret(T::*method)(Params...)>
    constexpr static Ret callback_method(CallablePtr obj, Params... params)
    {
        auto p = reinterpret_cast<T*>(obj);
        return (p->*method)(std::forward<Params>(params)...);
    }

    template<typename T, Ret(T::*method)(Params...) const>
    constexpr static Ret callback_const_method(CallablePtr obj, Params... params)
    {
        auto p = reinterpret_cast<const T*>(obj);
        return (p->*method)(std::forward<Params>(params)...);
    }
};

}
