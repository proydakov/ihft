#include <main.h>
#include <heap_data.h>
#include <memory/stream_fixed_pool_allocator.h>
#include <channel/one2many_seqnum_stream_object_queue.h>

int main(int argc, char* argv[])
{
    using allocator_t = ihft::stream_fixed_pool_allocator<std::uint64_t>;
    using queue_t = one2many_seqnum_stream_object_queue<data_t<allocator_t>, allocator_t>;

    std::cout << "sizeof(): " << sizeof(queue_t) << std::endl;

    return test_main<queue_t, perf_allocated_test<allocator_t>>(argc, argv);
}
