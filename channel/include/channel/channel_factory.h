#pragma once

#include <memory>
#include <vector>
#include <optional>

namespace ihft::channel
{

class channel_factory final
{
public:
    template<typename queue_t>
    struct producer_and_consumers final
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

        template <size_t I>
        auto& get() &
        {
            static_assert(I < 2);
            if constexpr (I == 0) return producer;
            else if constexpr (I == 1) return consumers;
        }

        queue_t producer;
        std::vector<typename queue_t::reader_type> consumers;
    };

    template<typename queue_t>
    using pc_t = producer_and_consumers<queue_t>;

    template<typename queue_t>
    using opc_t = std::optional<pc_t<queue_t>>;

    // factory methods

    template<typename queue_t>
    static opc_t<queue_t> make(std::size_t queue_capacity, std::size_t readers_count)
    {
        if (0 == readers_count)
        {
            return std::nullopt;
        }

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

    template<typename queue_t, typename ca_t, typename d_t = std::default_delete<ca_t>>
    static opc_t<queue_t> make(size_t queue_capacity, size_t readers_count, std::unique_ptr<ca_t, d_t> content_allocator)
    {
        if (0 == readers_count)
        {
            return std::nullopt;
        }

        auto result = std::make_optional<pc_t<queue_t>>(queue_capacity, std::move(content_allocator));
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
