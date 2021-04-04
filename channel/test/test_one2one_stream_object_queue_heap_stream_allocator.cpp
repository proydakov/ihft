#include <main.h>
#include <heap_data.h>
#include <channel/one2one_stream_object_queue.h>
#include <sys/mman.h>

namespace
{
    template<typename T>
    class stream_allocator_demo
    {
        struct alignas(ihft::QUEUE_CPU_CACHE_LINE_SIZE) holder
        {
            T data;
        };

        std::unique_ptr<holder[]> m_data;
        std::size_t m_next;
        std::size_t const m_size;

    public:
        using value_type = T;

    public:
        stream_allocator_demo(std::size_t pool_size)
            : m_data(new holder[pool_size])
            , m_next(0)
            , m_size(pool_size)
        {
            auto size = pool_size * sizeof(holder);

            // Просто включите hugepages…
            madvise(m_data.get(), size, MADV_HUGEPAGE);
            // … и задайте следующее
            madvise(m_data.get(), size, MADV_HUGEPAGE | MADV_SEQUENTIAL);
        }

        T* allocate(std::size_t)
        {
            auto& res = m_data[m_next];
            m_next = (++m_next) % m_size;
            return &res.data;
        }

        void deallocate(T*, std::size_t)
        {
        }
    };
}

int main(int argc, char* argv[])
{
    using allocator_t = stream_allocator_demo<std::uint64_t>;
    using queue_t = one2one_stream_object_queue<data_t<allocator_t>, allocator_t>;
    return test_main<queue_t, perf_allocated_test<allocator_t>, true>(argc, argv);
}
