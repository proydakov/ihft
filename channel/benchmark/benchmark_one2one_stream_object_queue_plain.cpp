#include <main.h>
#include <plain_data.h>
#include <channel/one2one_seqnum_stream_object_queue.h>

int main(int argc, char* argv[])
{
    return test_main<one2one_seqnum_stream_object_queue<data_t>, perf_plain_test, true>(argc, argv);
}
