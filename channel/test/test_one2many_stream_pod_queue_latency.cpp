#include <main.h>
#include <test_latency.h>
#include <channel/one2many_stream_pod_queue.h>

int main(int argc, char* argv[])
{
    return test_main<one2many_stream_pod_queue<data_t>, latency_test>(argc, argv);
}
