#pragma once

#include <logger/logger_level.h>
#include <logger/logger_contract.h>
#include <logger/logger_extra_data.h>
#include <logger/private/source_location.h>

#include <constant/constant.h>

#include <array>
#include <tuple>
#include <chrono>
#include <memory>
#include <iomanip>
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

    using time_point_t = std::chrono::time_point<std::chrono::system_clock>;

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

    void set_log_point_info(log_level level, time_point_t now, impl::source_location loc)
    {
        header.m_level = level;
        header.m_now = now;
        header.m_file = loc.m_file;
        header.m_func = loc.m_func;
        header.m_line = loc.m_line;
    }

    void set_thread_info(long id, char const (&tname)[16])
    {
        header.m_thread_id = id;
        static_assert(sizeof(tname) == sizeof(header.m_thread_name));
        for (size_t i = 0; i < sizeof(tname) - 1; i++)
        {
            header.m_thread_name[i] = tname[i];
        }
        header.m_thread_name[15] = '\0';
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

        static void time_point_to_stream(std::ostream& os, time_point_t tp)
        {
            const auto tt = tp.time_since_epoch();
            const time_t durS = std::chrono::duration_cast<std::chrono::seconds>(tt).count();
            if (const std::tm * tm = (std::gmtime(&durS))) {
                os << std::put_time(tm, "%Z %Y-%m-%d %H:%M:%S.");
                const auto durMs = std::chrono::duration_cast<std::chrono::microseconds>(tt).count();
                os << std::setw(3) << std::setfill('0') << static_cast<long>(durMs - durS * 1'000'000) << " ";
            }
        }

        void print_header(std::ostream& os) const
        {
            time_point_to_stream(os, m_now);

            os << m_level << " ";

            if (m_thread_id)
            {
                os << "[" << m_thread_name << ":" << m_thread_id << "] ";
            }

            os << m_file << '('
                << m_line << "):"
                << m_func << " "
            ;
        }

        void print_args_to(std::ostream& stream) const
        {
            if (not m_file.empty())
            {
                print_header(stream);
            }

            if (data_ptr and print_function)
            {
                print_function(data_ptr, format_expr, stream);
            }
        }

    public:
        log_level m_level = log_level::_NONE_;
        time_point_t m_now;

        long m_thread_id = 0;
        char m_thread_name[16];

        std::string_view m_file;
        std::string_view m_func;
        unsigned m_line = 0;

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
