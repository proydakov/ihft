#include <main.h>
#include <data_heap.h>
#include <channel/one2each_seqnum_stream_object_queue.h>

using namespace ihft::channel;

int main(int const argc, char const* argv[])
{
    using allocator_t = std::allocator<std::uint64_t>;
    using queue_t = one2each_seqnum_stream_object_queue<data_t<allocator_t>, allocator_t>;
    return test_main<queue_t, perf_allocated_test<allocator_t>>(argc, argv);
}
