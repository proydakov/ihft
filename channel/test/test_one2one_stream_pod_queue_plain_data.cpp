#include <main.h>
#include <test_plain_data.h>
#include <channel/one2one_stream_pod_queue.h>

int main(int argc, char* argv[])
{
    return test_main<one2one_stream_pod_queue<data_t>, perf_plain_test, true>(argc, argv);
}
