#pragma once

#include <constant/constant.h>
#include <platform/platform.h>

#include <string>
#include <chrono>
#include <vector>
#include <limits>
#include <fstream>
#include <algorithm>

using clock_type = std::chrono::steady_clock;
using time_point_t = std::chrono::time_point<clock_type>;

struct data_t
{
    data_t(time_point_t start) noexcept
        : m_start(std::move(start))
    {
    }

    time_point_t m_start;
};

struct latency_test
{
    static constexpr unsigned default_delta = std::numeric_limits<unsigned>::min();
    static constexpr unsigned max_pow_2 = 32;
    static constexpr unsigned mask = max_pow_2 - 1;

    latency_test(std::size_t NUM_READERS, std::size_t TOTAL_EVENTS) noexcept
    {
        m_lines.resize(NUM_READERS);
        for(auto & line : m_lines)
        {
            line.m_delta.reserve(TOTAL_EVENTS / max_pow_2);
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
        return data_t(clock_type::now());
    }

    void check_data(std::uint64_t reader_id, data_t const& data)
    {
        auto const end = clock_type::now();
        auto const delta = static_cast<unsigned>((end - data.m_start).count());

        auto& result = m_lines[reader_id];
        result.val = std::max(result.val, delta);
        if (result.tick == mask)
        {
            result.m_delta.emplace_back( result.val );
            result.val = default_delta;
        }
        result.tick++;
        result.tick &= mask;
    }

    void reader_done() noexcept
    {
    }

    void writer_done() noexcept
    {
    }

private:
    struct alignas(constant::CPU_CACHE_LINE_SIZE) line_t
    {
        unsigned tick{};
        unsigned val{default_delta};
        std::vector<unsigned> m_delta;
    };

    std::vector<line_t> m_lines;
};
