#pragma once

#include <constant/constant.h>
#include <platform/platform.h>

#include <atomic>
#include <memory>
#include <cstdint>
#include <iostream>

struct alignas(ihft::constant::CPU_CACHE_LINE_SIZE) stat_local_t
{
    std::int64_t counter{0};
};

struct alignas(ihft::constant::CPU_CACHE_LINE_SIZE) stat_global_t
{
    std::atomic<std::int64_t> counter{0};
};

thread_local stat_local_t g_local_allocated;
thread_local stat_local_t g_local_released;

template<typename allocator_t>
struct data_t
{
    using value_type = typename allocator_t::value_type;

    data_t(std::uint64_t val, allocator_t& allocator) noexcept
        : m_ptr(std::construct_at(allocator.allocate(1), val))
        , m_allocator(allocator)
    {
        g_local_allocated.counter++;
    }

    data_t(data_t&& data) noexcept
        : m_ptr(data.m_ptr)
        , m_allocator(data.m_allocator)
    {
        data.m_ptr = nullptr;
    }

    data_t& operator=(data_t&& data) = delete;
    data_t(const data_t&) = delete;
    data_t& operator=(const data_t&) = delete;

    ~data_t() noexcept
    {
        if (m_ptr != nullptr)
        {
            std::destroy_at(m_ptr);
            m_allocator.deallocate(m_ptr, 1);
            m_ptr = nullptr;
            g_local_released.counter++;
        }
    }

    typename std::allocator_traits<allocator_t>::pointer m_ptr;
    allocator_t& m_allocator;
};

template<typename allocator_t>
struct perf_allocated_test
{
    static constexpr bool flush = false;

    perf_allocated_test(std::uint64_t, std::uint64_t, allocator_t& allocator) noexcept
        : m_allocator(allocator)
    {
        std::cout << "g_counter before: " << (m_global_allocated.counter - m_global_released.counter) << " (must be zero)" << std::endl;
    }

    ~perf_allocated_test() noexcept
    {
        std::cout << "g_counter after: " << (m_global_allocated.counter - m_global_released.counter) << " (must be zero)" << std::endl;
    }

    auto create_data(std::uint64_t i) noexcept
    {
        return data_t<allocator_t>(i, m_allocator);
    }

    void check_data(std::uint64_t, std::uint64_t i, data_t<allocator_t> const& cref) noexcept
    {
        if (i != *cref.m_ptr)
        {
            abort();
        }
    }

    void reader_done() noexcept
    {
        m_global_released.counter += g_local_released.counter;
    }

    void writer_done() noexcept
    {
        m_global_allocated.counter += g_local_allocated.counter;
    }

    allocator_t& m_allocator;
    stat_global_t m_global_allocated;
    stat_global_t m_global_released;
};
