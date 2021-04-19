#pragma once

#include <vector>
#include <memory>
#include <optional>

namespace ihft
{

class channel_factory
{
public:
    template<typename queue_t>
    struct producer_and_consumers
    {
        producer_and_consumers(std::size_t n)
            : producer(n)
        {
        }

        template<typename content_allocator_t, typename deleter_t = std::default_delete<content_allocator_t>>
        producer_and_consumers(std::size_t n, std::unique_ptr<content_allocator_t, deleter_t> content_allocator)
            : producer(n, std::move(content_allocator))
        {
        }

        queue_t producer;

        std::vector<typename queue_t::reader_type> consumers;
    };

    template<typename queue_t>
    static std::optional<producer_and_consumers<queue_t>> make(std::size_t queue_capacity, std::size_t readers_count)
    {
        auto result = std::make_optional<producer_and_consumers<queue_t>>(queue_capacity);
        if (not result)
        {
            return std::nullopt;
        }
        auto& pair = *result;
        for (std::size_t i = 0; i < readers_count; i++)
        {
            auto reader = pair.producer.create_reader();
            if (not reader)
            {
                return std::nullopt;
            }
            pair.consumers.emplace_back(std::move(*reader));
        }
        return result;
    }

    template<typename queue_t, typename content_allocator_t, typename deleter_t = std::default_delete<content_allocator_t>>
    static std::optional<producer_and_consumers<queue_t>> make(std::size_t queue_capacity, std::size_t readers_count, std::unique_ptr<content_allocator_t, deleter_t> content_allocator)
    {
        auto result = std::make_optional<producer_and_consumers<queue_t>>(queue_capacity, std::move(content_allocator));
        if (not result)
        {
            return std::nullopt;
        }
        auto& pair = *result;
        for (std::size_t i = 0; i < readers_count; i++)
        {
            auto reader = pair.producer.create_reader();
            if (not reader)
            {
                return std::nullopt;
            }
            pair.consumers.emplace_back(std::move(*reader));
        }
        return result;
    }
};

}
