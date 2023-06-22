#include <logger/logger.h>

/*

UTC 2023-06-22 20:54:32.830756 DEBUG [main:1580096] logger_simple.cpp(14):int main() Hello world !!!
UTC 2023-06-22 20:54:32.832750  INFO [main:1580096] logger_simple.cpp(15):int main() Hello world !!!
UTC 2023-06-22 20:54:32.832768  WARN [main:1580096] logger_simple.cpp(16):int main() Hello world !!!
UTC 2023-06-22 20:54:32.832783 ERROR [main:1580096] logger_simple.cpp(17):int main() Hello world !!!

*/

int main()
{
    IHFT_LOG_DEBUG("Hello {} !!!", "world");
    IHFT_LOG_INFO("Hello {} !!!", "world");
    IHFT_LOG_WARN("Hello {} !!!", "world");
    IHFT_LOG_ERROR("Hello {} !!!", "world");

    return 0;
}
