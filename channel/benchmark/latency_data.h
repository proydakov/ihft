#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <fstream>

#include "platform/platform.h"

using time_point = std::chrono::time_point<std::chrono::steady_clock>;

struct data_t
{
    data_t(time_point start) noexcept
        : m_start(std::move(start))
    {
    }

    std::chrono::time_point<std::chrono::steady_clock> m_start;
};

struct latency_test
{
    latency_test(std::size_t NUM_READERS, std::size_t TOTAL_EVENTS) noexcept
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
            for(auto const delta : line.m_delta)
            {
                output << delta << "\n";
            }
        }
    }

    data_t create_data(std::uint64_t) noexcept
    {
        return data_t(std::chrono::steady_clock::now());
    }

    void check_data(std::uint64_t reader_id, data_t const& data)
    {
        auto const end = std::chrono::steady_clock::now();
        m_lines[reader_id].m_delta.emplace_back( (end - data.m_start).count() );
    }

    void reader_done() noexcept
    {
    }

    void writer_done() noexcept
    {
    }

private:
    struct alignas(platform::CPU_CACHE_LINE_SIZE) line_t
    {
        std::vector<unsigned> m_delta;
    };

    std::vector<line_t> m_lines;
};
