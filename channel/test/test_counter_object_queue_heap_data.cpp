#include <main.h>
#include <test_heap_data.h>
#include <channel/one2many_counter_object_queue.h>

int main(int argc, char* argv[])
{
    using allocator_t = std::allocator<std::uint64_t>;
    using queue_t = one2many_counter_object_queue<data_t<allocator_t>, allocator_t>;
    return test_main<queue_t, perf_allocated_test<allocator_t>>(argc, argv);
}
