#include <main.h>
#include <data_latency.h>
#include <channel/one2each_seqnum_stream_object_queue.h>

using namespace ihft::channel;

int main(int const argc, char const* argv[])
{
    return test_main<one2each_seqnum_stream_object_queue<data_t>, latency_test>(argc, argv);
}
