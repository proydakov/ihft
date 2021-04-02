#include <main.h>
#include <test_plain_data.h>
#include <channel/one2many_stream_object_queue.h>

int main(int argc, char* argv[])
{
    return test_main<one2many_stream_object_queue<data_t>, perf_plain_test>(argc, argv);
}
