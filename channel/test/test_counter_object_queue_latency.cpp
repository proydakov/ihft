#include <main.h>
#include <test_latency.h>
#include <channel/one2many_counter_object_queue.h>

int main(int argc, char* argv[])
{
    return test_main<one2many_counter_object_queue<data_t, std::uint64_t>, latency_test>(argc, argv);
}
