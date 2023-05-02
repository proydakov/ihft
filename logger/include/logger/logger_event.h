#pragma once

#include <logger/logger_contract.h>
#include <logger/logger_extra_data.h>

#include <constant/constant.h>

#include <array>
#include <tuple>
#include <memory>
#include <ostream>
#include <utility>
#include <string_view>

namespace ihft::logger
{

///
/// The cornerstone of logging system.
/// It represents a smart arguments wrapper and produce an instruction.
/// how to pack origin arguments into internal buffer.
///

struct alignas(constant::CPU_CACHE_LINE_SIZE) logger_event final
{
    static constexpr size_t ITEM_SIZE = 4096;

private:
    template<typename Tuple, typename Array, std::size_t... Is>
    static void print_tuple_impl(std::ostream& os, Array const& a, Tuple const& t, std::index_sequence<Is...>)
    {
        ((os << a[Is] << std::get<Is>(t)), ...);
        os << a.back();
    }

    template<typename Tuple>
    static void print_tuple(std::ostream& os, std::string_view expr, Tuple const& t)
    {
        constexpr std::size_t tuple_size = std::tuple_size_v<Tuple>;
        constexpr std::size_t array_size = tuple_size + 1; // need extra place for tail

        std::array<std::string_view, array_size> array;
        size_t from{}, count{}, pos{};

        while(pos = expr.find("{}", from), pos != std::string_view::npos and count < tuple_size)
        {
            array[count] = expr.substr(from, pos - from);
            count += 1;
            pos += 2;
            from = pos;
        }

        array[count] = expr.substr(from);

        using indices = std::make_index_sequence<tuple_size>;
        print_tuple_impl(os, array, t, indices{});
    }

    template<typename T>
    static void print(void* raw_ptr, std::string_view format_expr, std::ostream& os)
    {
        auto data_ptr = reinterpret_cast<T*>(raw_ptr);
        print_tuple(os, format_expr, *data_ptr);
    }

    template<typename T>
    static void clean(void* raw_ptr) noexcept
    {
        auto data_ptr = reinterpret_cast<T*>(raw_ptr);
        std::destroy_at(data_ptr);
    }

    template<typename T>
    static consteval bool check_type()
    {
        return not std::is_same_v<typename logger_contract<T>::type, void>;
    }

public:
    template<typename ... Args>
    logger_event(std::string_view format_expr, Args&& ... args)
    {
        // Assign a common header information
        header.format_expr = format_expr;

        // Check the input types list
        static_assert((check_type<std::remove_cvref_t<Args>>() && ...), "Please overload logger_contract for a new type.");

        // Serrialize the arguments list into buffer
        if constexpr ((check_type<std::remove_cvref_t<Args>>() && ...))
        {
            using result_t = std::tuple<typename logger_contract<std::remove_cvref_t<Args>>::type ...>;

            static_assert(sizeof(result_t) <= sizeof(buffer), "Input is too big.");
            static_assert(alignof(buffer_t) % alignof(result_t) == 0, "Input has unsupportable alignment.");

            auto memory_ptr = reinterpret_cast<result_t*>(&buffer);
            auto extra_data_ptr = static_cast<char*>(buffer.data) + sizeof(result_t);
            constexpr size_t extra_data_size = sizeof(buffer_t) - sizeof(result_t);
            logger_extra_data extra_data_ctx(extra_data_ptr, extra_data_size);

            header.data_ptr = std::construct_at(memory_ptr, logger_contract<std::remove_cvref_t<Args>>::pack(extra_data_ctx, std::forward<Args>(args)) ...);
            header.print_function = &print<result_t>;
            header.clean_function = &clean<result_t>;
        }
    }

    logger_event(const logger_event&) = delete;
    logger_event(logger_event&&) noexcept = delete;

    logger_event& operator=(const logger_event&) = delete;
    logger_event& operator=(logger_event&&) noexcept = delete;

    ~logger_event() noexcept
    {
    }

    void print_args_to(std::ostream& stream) const
    {
        header.print_args_to(stream);
    }

private:
    using print_function_t = void (*)(void*, std::string_view, std::ostream&);
    using clean_function_t = void (*)(void*);

    struct alignas(constant::CPU_CACHE_LINE_SIZE) header_t final
    {
        ~header_t() noexcept
        {
            if (data_ptr and clean_function)
            {
                clean_function(data_ptr);
            }
        }

        void print_args_to(std::ostream& stream) const
        {
            if (data_ptr and print_function)
            {
                print_function(data_ptr, format_expr, stream);
            }
        }

        std::string_view format_expr;
        void* data_ptr = nullptr;
        print_function_t print_function = nullptr;
        clean_function_t clean_function = nullptr;
    };

    struct alignas(constant::CPU_CACHE_LINE_SIZE) buffer_t final
    {
        char data[ITEM_SIZE - sizeof(header_t)];
    };

private:
    header_t header;
    buffer_t buffer;
};

static_assert(alignof(logger_event) == constant::CPU_CACHE_LINE_SIZE, "Logger event should use a CPU CACHE LINE alignment.");
static_assert(sizeof(logger_event) == logger_event::ITEM_SIZE, "Logger event should use a single memory page.");

}
