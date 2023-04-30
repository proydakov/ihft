#include <catch2/catch.hpp>
#include <constant/constant.h>
#include <logger/logger_contract.h>
#include <logger/logger_extra_data.h>

#include <tuple>
#include <string>
#include <utility>
#include <sstream>
#include <cstddef>
#include <type_traits>

using namespace ihft::logger;

// Main class.
// It represents a smart arguments wrapper and produce an instruction
// how to pack origin arguments into internal buffer
struct alignas(ihft::constant::CPU_CACHE_LINE_SIZE) logger_event final
{
    static constexpr size_t ITEM_SIZE = 1024;

private:
    template<typename T>
    static consteval bool check_type()
    {
        return !std::is_same_v<typename logger_contract<T>::type, void>;
    }

    template<typename Tuple, std::size_t... Is>
    static void print_tuple_impl(std::ostream& os, Tuple const& t, std::index_sequence<Is...>)
    {
        os << "(";
        ((os << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
        os << ")";
    }

    template<typename Tuple>
    static void print_tuple(std::ostream& os, Tuple const& t)
    {
        constexpr auto tuple_size = std::tuple_size_v<Tuple>;
        using indices = std::make_index_sequence<tuple_size>;
        print_tuple_impl(os, t, indices{});
    }

    template<typename T>
    static void print(void* raw_ptr, std::ostream& os)
    {
        auto data_ptr = reinterpret_cast<T*>(raw_ptr);
        print_tuple(os, *data_ptr);
    }

    template<typename T>
    static void clean(void* raw_ptr)
    {
        auto data_ptr = reinterpret_cast<T*>(raw_ptr);
        std::destroy_at(data_ptr);
    }

public:
    template<typename ... Args>
    logger_event(Args&& ... args)
    {
        static_assert((check_type<std::remove_cvref_t<Args>>() && ...), "Please overload logger_contract for a new type.");

        if constexpr ((check_type<std::remove_cvref_t<Args>>() && ...))
        {
            using result_t = std::tuple<typename logger_contract<std::remove_cvref_t<Args>>::type ...>;

            static_assert(sizeof(result_t) <= sizeof(buffer), "Input is too big.");
            static_assert(alignof(buffer_t) % alignof(result_t) == 0, "Input has unsupportable alignment.");

            auto memory_ptr = reinterpret_cast<result_t*>(&buffer);
            auto extra_data_ptr = static_cast<char*>(buffer.data) + sizeof(result_t);
            constexpr size_t extra_data_size = sizeof(buffer_t) - sizeof(result_t);
            logger_extra_data extra_data_ctx(extra_data_ptr, extra_data_size);

            header.data_ptr = std::construct_at(memory_ptr, logger_contract<std::remove_cvref_t<Args>>::pack(extra_data_ctx, args) ...);
            header.print_function = &print<result_t>;
            header.clean_function = &clean<result_t>;
        }
    }

    logger_event(const logger_event&) = delete;
    logger_event(logger_event&&) noexcept = delete;

    logger_event& operator=(const logger_event&) = delete;
    logger_event& operator=(logger_event&&) noexcept = delete;

    void print_to(std::ostream& stream)
    {
        header.print_to(stream);
    }

    using print_function_t = void (*)(void*, std::ostream&);
    using clean_function_t = void (*)(void*);

    struct alignas(ihft::constant::CPU_CACHE_LINE_SIZE) header_t final
    {
        ~header_t()
        {
            if (data_ptr && clean_function)
            {
                clean_function(data_ptr);
            }
        }

        void print_to(std::ostream& stream)
        {
            if (data_ptr && print_function)
            {
                print_function(data_ptr, stream);
            }
        }

        void* data_ptr = nullptr;
        print_function_t print_function = nullptr;
        clean_function_t clean_function = nullptr;
    };

    struct alignas(ihft::constant::CPU_CACHE_LINE_SIZE) buffer_t final
    {
        char data[ITEM_SIZE - sizeof(header_t)];
    };

private:
    header_t header;
    buffer_t buffer;
};

static_assert(sizeof(logger_event::header_t) == 64);
static_assert(sizeof(logger_event::buffer_t) == logger_event::ITEM_SIZE - 64);
static_assert(sizeof(logger_event) == logger_event::ITEM_SIZE, "Logger event should use a single memory page.");

// Unit tests

TEST_CASE("plain by value")
{
    logger_event event(long{1024}, float{3.14f}, char{'A'});

    std::ostringstream sstream;
    event.print_to(sstream);

    REQUIRE(sstream.str() == "(1024, 3.14, A)");
}

TEST_CASE("plain by reference")
{
    auto ilong = unsigned{512};
    auto iflot = double{1.57f};
    auto ichar = char{'c'};

    logger_event event(ilong, iflot, ichar);

    std::ostringstream sstream;
    event.print_to(sstream);

    REQUIRE(sstream.str() == "(512, 1.57, c)");
}

TEST_CASE("plain by const reference")
{
    auto const ilong = int{256};
    auto const iflot = double{7.77};
    auto const ichar = char{'Y'};

    logger_event event(ilong, iflot, ichar);

    std::ostringstream sstream;
    event.print_to(sstream);

    REQUIRE(sstream.str() == "(256, 7.77, Y)");
}

TEST_CASE("string by value")
{
    logger_event event(std::string{"IHFT"}, std::string{"C++"}, std::string_view{"zone"});

    std::ostringstream sstream;
    event.print_to(sstream);

    REQUIRE(sstream.str() == "(IHFT, C++, zone)");
}

TEST_CASE("string by reference")
{
    std::string str1{"hello"};
    std::string str2{"world"};
    std::string_view view{"C++"};

    logger_event event(str1, str2, view);

    std::ostringstream sstream;
    event.print_to(sstream);

    REQUIRE(sstream.str() == "(hello, world, C++)");
}

TEST_CASE("string by const reference")
{
    std::string const str1{"Alice's"};
    std::string const str2{"Adventures"};
    std::string_view const view1{"in"};
    std::string_view const view2{"Wonderland"};

    logger_event event(str1, str2, view1, view2);

    std::ostringstream sstream;
    event.print_to(sstream);

    REQUIRE(sstream.str() == "(Alice's, Adventures, in, Wonderland)");
}

TEST_CASE("char array by value")
{
    logger_event event("ARRAY", "is", "HERE");

    std::ostringstream sstream;
    event.print_to(sstream);

    REQUIRE(sstream.str() == "(ARRAY, is, HERE)");
}

TEST_CASE("char array by reference")
{
    char arr1[4] = "C++";
    char arr2[3] = "20";

    logger_event event(arr1, arr2);

    std::ostringstream sstream;
    event.print_to(sstream);

    REQUIRE(sstream.str() == "(C++, 20)");
}

TEST_CASE("char array by const reference")
{
    const char arr1[5] = "TEST";
    const char arr2[10] = "OPERATION";

    logger_event event(arr1, arr2);

    std::ostringstream sstream;
    event.print_to(sstream);

    REQUIRE(sstream.str() == "(TEST, OPERATION)");
}

TEST_CASE("nullptr_t")
{
    logger_event event(nullptr);

    std::ostringstream sstream;
    event.print_to(sstream);

    auto const str = sstream.str();
    REQUIRE((str == "(0)" || str == "(0x0)" || str == "((nil))"));
}

TEST_CASE("pointer")
{
    void* ptr = nullptr;

    logger_event event(ptr);

    std::ostringstream sstream;
    event.print_to(sstream);

    auto const str = sstream.str();
    REQUIRE((str == "(0)" || str == "(0x0)" || str == "((nil))"));
}

TEST_CASE("const pointer")
{
    const void* ptr = nullptr;

    logger_event event(ptr);

    std::ostringstream sstream;
    event.print_to(sstream);

    auto const str = sstream.str();
    REQUIRE((str == "(0)" || str == "(0x0)" || str == "((nil))"));
}
