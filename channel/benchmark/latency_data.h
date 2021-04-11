#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <fstream>

#include "channel/common.h"

struct data_t
{
    data_t(std::uint64_t start) noexcept
        : m_start(start)
    {
    }

    std::uint64_t m_start;
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
            for(auto delta : line.m_delta)
            {
                output << delta << "\n";
            }
        }
    }

    data_t create_data(std::uint64_t) noexcept
    {
        return data_t(__rdtsc());
    }

    void check_data(std::uint64_t reader_id, data_t const& data)
    {
        auto const end = __rdtsc();
        m_lines[reader_id].m_delta.push_back(end - data.m_start);
    }

    void reader_done() noexcept
    {
    }

    void writer_done() noexcept
    {
    }

private:
    struct alignas(channel::CPU_CACHE_LINE_SIZE) line_t
    {
        std::vector<std::uint64_t> m_delta;
    };

    std::vector<line_t> m_lines;
};
