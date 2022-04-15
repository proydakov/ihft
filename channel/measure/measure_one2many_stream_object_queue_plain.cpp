#include <main.h>
#include <plain_data.h>
#include <channel/one2many_seqnum_stream_object_queue.h>

using namespace ihft::channel;

int main(int argc, char* argv[])
{
    return test_main<one2many_seqnum_stream_object_queue<data_t>, perf_plain_test>(argc, argv);
}
