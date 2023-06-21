#include <catch2/catch.hpp>

#include <types/function_ref.h>

#include <iostream>

using callback_t = ihft::types::function_ref<int(int)>;

int function(int val)
{
    std::cout << "function(): " << val << "\n";
    return val;
}

TEST_CASE("function")
{
    callback_t const delayed_call = function;

    for(int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("function address")
{
    callback_t const delayed_call = &function;

    for(int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

struct helper
{
    static int function(int val)
    {
        std::cout << "helper::function(): " << val << "\n";
        return val;
    }
};

TEST_CASE("static function")
{
    callback_t const delayed_call = helper::function;

    for(int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("static function address")
{
    callback_t const delayed_call = &helper::function;

    for(int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("functor")
{
    auto functor = [](int val) mutable
    {
        std::cout << "mutable functor(): " << val << std::endl;
        return val;
    };

    callback_t const delayed_call = functor;
    for(int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("functor address")
{
    auto functor = [](int val) mutable
    {
        return val;
    };

    static_assert(!std::is_constructible_v<callback_t, decltype(&functor)>);
}

TEST_CASE("const functor")
{
    {
        auto functor = [](int val)
        {
            std::cout << "functor(): " << val << std::endl;
            return val;
        };

        callback_t const delayed_call = functor;
        for(int i = 1; i <= 1024; i *= 2)
        {
            REQUIRE(delayed_call(i) == i);
        }
    }

    {
        auto const functor = [](int val)
        {
            std::cout << "functor(): " << val << std::endl;
            return val;
        };

        callback_t const delayed_call = functor;
        for(int i = 1; i <= 1024; i *= 2)
        {
            REQUIRE(delayed_call(i) == i);
        }
    }
}

TEST_CASE("const functor address")
{
    {
        auto functor = [](int val)
        {
            return val;
        };

        static_assert(!std::is_constructible_v<callback_t, decltype(&functor)>);
    }

    {
        auto const functor = [](int val)
        {
            return val;
        };

        static_assert(!std::is_constructible_v<callback_t, decltype(&functor)>);
    }
}

TEST_CASE("class operator()")
{
    struct i_object
    {
        int operator()(int val)
        {
            std::cout << "i_object_call::operator(): " << val << "\n";
            return val;
        }

        int operator()(int val) const
        {
            FAIL("newer call me");
            return val;
        }
    } object;

    callback_t const delayed_call = object;
    for(int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("class const operator()")
{
    struct i_object
    {
        int operator()(int val)
        {
            FAIL("newer call me");
            return val;
        }

        int operator()(int val) const
        {
            std::cout << "i_object::operator() const: " << val << "\n";
            return val;
        }
    } const object;

    callback_t const delayed_call = object;
    for(int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("copy")
{
    callback_t delayed_call;
    REQUIRE(!delayed_call);

    {
        callback_t c2 = function;
        REQUIRE(c2);

        delayed_call = c2;
        REQUIRE(delayed_call);
        REQUIRE(c2);
    }

    for(int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("move")
{
    callback_t delayed_call;
    REQUIRE(!delayed_call);

    {
        callback_t c2 = function;
        REQUIRE(c2);

        delayed_call = std::move(c2);
        REQUIRE(delayed_call);
    }

    for(int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("swap")
{
    callback_t f1 = function;
    callback_t f2;

    REQUIRE(f1);
    REQUIRE(!f2);

    REQUIRE(f1(7) == 7);

    swap(f1, f2);

    REQUIRE(!f1);
    REQUIRE(f2);

    REQUIRE(f2(7) == 7);
}

TEST_CASE("call by valye : move vs copy")
{
    static int i_ctor{};
    static int i_copy{};
    static int i_move{};
    static int i_dtor{};

    struct data_t final
    {
        explicit data_t(int data) : m_data(data) { i_ctor++; }
        ~data_t() { i_dtor++; }

        data_t(data_t const& other) : m_data(other.m_data) { i_copy++; }
        data_t(data_t&& other) noexcept : m_data(other.m_data) { i_move++; }

        static int call(data_t data)
        {
            std::cout << "data_t::call() " << data.m_data << "\n";
            return data.m_data;
        }

        int m_data;
    };

    using datacall_t = ihft::types::function_ref<int(data_t)>;

    datacall_t callback = data_t::call;

    REQUIRE(i_ctor == 0);
    REQUIRE(i_copy == 0);
    REQUIRE(i_move == 0);
    REQUIRE(i_dtor == 0);

    {
        data_t d(777);
        REQUIRE(callback(d) == 777);
    }

    REQUIRE(i_ctor == 1);
    REQUIRE(i_copy == 1);
    REQUIRE(i_move == 2);
    REQUIRE(i_dtor == 4);
}

TEST_CASE("call by reference")
{
    static int i_ctor{};
    static int i_copy{};
    static int i_move{};
    static int i_dtor{};

    struct ref_t final
    {
        explicit ref_t(int data) : m_data(data) { i_ctor++; }
        ~ref_t() { i_dtor++; }

        ref_t(ref_t const& other) : m_data(other.m_data) { i_copy++; }
        ref_t(ref_t&& other) noexcept : m_data(other.m_data) { i_move++; }

        static int call(ref_t const& data)
        {
            std::cout << "ref_t::call() " << data.m_data << "\n";
            return data.m_data;
        }

        int m_data;
    };

    using datacall_t = ihft::types::function_ref<int(ref_t const&)>;

    datacall_t callback = ref_t::call;

    REQUIRE(i_ctor == 0);
    REQUIRE(i_copy == 0);
    REQUIRE(i_move == 0);
    REQUIRE(i_dtor == 0);

    {
        ref_t d(888);
        REQUIRE(callback(d) == 888);
    }

    REQUIRE(i_ctor == 1);
    REQUIRE(i_copy == 0);
    REQUIRE(i_move == 0);
    REQUIRE(i_dtor == 1);
}
