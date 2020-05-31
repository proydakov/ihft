#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <fstream>

#include "common.h"

struct data_t
{
    data_t(std::chrono::time_point<std::chrono::high_resolution_clock> start) noexcept : m_start(start)
    {
    }

    ~data_t() noexcept
    {
    }

    data_t(data_t&& data) noexcept
        : m_start(data.m_start)
    {
    }

    data_t& operator=(data_t&& data) = delete;
    data_t(const data_t&) = delete;
    void operator=(const data_t&) = delete;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

struct latency_test
{
    template<typename allocator_t>
    latency_test(std::size_t NUM_READERS, std::size_t TOTAL_EVENTS, allocator_t&) noexcept
    {
        m_lines.resize(NUM_READERS);
        for(auto & line : m_lines)
        {
            line.m_delta.reserve(TOTAL_EVENTS);
        }
    }

    ~latency_test() noexcept
    {
        for(std::size_t i = 0; i < m_lines.size(); i++)
        {
            auto const& line = m_lines[i];
            std::ofstream output("reader_" + std::to_string(i));
            for(auto delta : line.m_delta)
            {
                output << delta << "\n";
            }
        }
    }

    data_t create_data(std::uint64_t) noexcept
    {
        return data_t(std::chrono::high_resolution_clock::now());
    }

    void check_data(std::size_t i, data_t const& data)
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> stop = std::chrono::high_resolution_clock::now();
        auto const microseconds = static_cast<long>(std::chrono::duration_cast<std::chrono::microseconds>(stop - data.m_start).count());
        m_lines[i].m_delta.push_back(microseconds);
    }

    void reader_done() noexcept
    {
    }

    void writer_done() noexcept
    {
    }

private:
    struct alignas(QUEUE_CPU_CACHE_LINE_SIZE) line_t
    {
        std::vector<long> m_delta;
    };

    std::vector<line_t> m_lines;
};
