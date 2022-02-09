#include <main.h>
#include <latency_data.h>
#include <channel/one2one_seqnum_stream_pod_queue.h>

using namespace ihft::channel;

int main(int argc, char* argv[])
{
    return test_main<one2one_seqnum_stream_pod_queue<data_t>, latency_test, true>(argc, argv);
}
