#include <main.h>
#include <data_plain.h>
#include <channel/one2each_seqnum_stream_pod_queue.h>

using namespace ihft::channel;

int main(int const argc, char const* argv[])
{
    return test_main<one2each_seqnum_stream_pod_queue<data_t>, perf_plain_test>(argc, argv);
}
