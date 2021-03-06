#pragma once

#include <cstdint>

struct data_t
{
    data_t() noexcept
        : m_value(0)
    {
    }

    data_t(std::uint64_t value) noexcept
        : m_value(value)
    {
    }

    std::uint64_t m_value;
};

struct perf_plain_test
{
    static constexpr bool flush = false;

    perf_plain_test(std::uint64_t, std::uint64_t) noexcept
    {
    }

    ~perf_plain_test() noexcept
    {
    }

    data_t create_data(std::uint64_t i) noexcept
    {
        return data_t(i);
    }

    void check_data(std::uint64_t, std::uint64_t i, data_t const& cref) noexcept
    {
        if (i != cref.m_value)
        {
            abort();
        }
    }

    void reader_done() noexcept
    {
    }

    void writer_done() noexcept
    {
    }
};
