#include <main.h>
#include <test_heap_data.h>
#include <one2many_bitmask_queue.h>

int main(int argc, char* argv[])
{
    using allocator_t = std::allocator<std::uint64_t>;
    using queue_t = one2many_bitmask_queue<data_t<allocator_t>, std::uint64_t, allocator_t>;
    return test_main<queue_t, perf_allocated_test<allocator_t>>(argc, argv);
}
