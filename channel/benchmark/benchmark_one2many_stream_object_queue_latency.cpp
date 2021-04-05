#include <main.h>
#include <latency_data.h>
#include <channel/one2many_stream_object_queue.h>

int main(int argc, char* argv[])
{
    return test_main<one2many_stream_object_queue<data_t>, latency_test>(argc, argv);
}
