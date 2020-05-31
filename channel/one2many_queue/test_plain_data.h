#pragma once

#include <cstdint>

struct data_t
{
    data_t() noexcept : m_value(0)
    {
    }

    ~data_t() noexcept
    {
    }

    data_t(std::uint64_t value) noexcept : m_value(value)
    {
    }

    data_t(data_t&& data) noexcept : m_value(data.m_value)
    {
        data.m_value = 0;
    }

    data_t& operator=(data_t&& data) = delete;
    data_t(const data_t&) = delete;
    void operator=(const data_t&) = delete;

    std::uint64_t m_value;
};

struct perf_plain_test
{
    template<typename allocator_t>
    perf_plain_test(std::uint64_t, std::uint64_t, allocator_t&) noexcept
    {
    }

    ~perf_plain_test() noexcept
    {
    }

    data_t create_data(std::uint64_t i) noexcept
    {
        return data_t(i);
    }

    void check_data(std::uint64_t, data_t const&) noexcept
    {
    }

    void reader_done() noexcept
    {
    }

    void writer_done() noexcept
    {
    }
};
