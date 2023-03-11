#include <main.h>
#include <data_latency.h>
#include <channel/one2one_seqnum_stream_pod_queue.h>

using namespace ihft::channel;

int main(int const argc, char const* argv[])
{
    return test_main<one2one_seqnum_stream_pod_queue<data_t>, latency_test, true>(argc, argv);
}
