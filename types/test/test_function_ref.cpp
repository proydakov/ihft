#include <catch2/catch.hpp>

#include <types/function_ref.h>

#include <iostream>

using callback = ihft::types::function_ref<int(int)>;

int function(int val)
{
    std::cout << "function(): " << val << "\n";
    return val;
}

TEST_CASE("function")
{
    auto const delayed_call = callback::function<&function>();
    for (int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("method")
{
    struct i_object_call
    {
        int call(int val)
        {
            std::cout << "i_object_call::call(): " << val << "\n";
            return val;
        }
    };

    i_object_call object;

    auto const delayed_call = callback::method<i_object_call, &i_object_call::call>(object);
    for (int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("const_method")
{
    struct i_object_call
    {
        int call(int val)
        {
            FAIL("newer call me");
            return val;
        }

        int call(int val) const
        {
            std::cout << "i_object_call::call() const: " << val << "\n";
            return val;
        }
    };

    {
        i_object_call object1;

        auto const delayed_call = callback::const_method<i_object_call, &i_object_call::call>(object1);
        for (int i = 1; i <= 1024; i *= 2)
        {
            REQUIRE(delayed_call(i) == i);
        }
    }

    {
        i_object_call const object2;

        auto const delayed_call = callback::const_method<i_object_call, &i_object_call::call>(object2);
        for (int i = 1; i <= 1024; i *= 2)
        {
            REQUIRE(delayed_call(i) == i);
        }
    }
}

TEST_CASE("functor_test")
{
    auto functor = [](int val) mutable
    {
        std::cout << "mutable functor(): " << val << std::endl;
        return val;
    };

    auto delayed_call = callback::functor(functor);
    for (int i = 1; i <= 1024; i *= 2)
    {
        REQUIRE(delayed_call(i) == i);
    }
}

TEST_CASE("const_functor_test")
{
    {
        auto functor = [](int val)
        {
            std::cout << "functor(): " << val << std::endl;
            return val;
        };

        auto delayed_call = callback::const_functor(functor);
        for (int i = 1; i <= 1024; i *= 2)
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

        auto delayed_call = callback::const_functor(functor);
        for (int i = 1; i <= 1024; i *= 2)
        {
            REQUIRE(delayed_call(i) == i);
        }
    }
}

TEST_CASE("swap")
{
    callback f1 = callback::function<&function>();
    callback f2;

    REQUIRE(f1);
    REQUIRE(!f2);

    REQUIRE(f1(7) == 7);

    std::swap(f1, f2);

    REQUIRE(!f1);
    REQUIRE(f2);

    REQUIRE(f2(7) == 7);
}
