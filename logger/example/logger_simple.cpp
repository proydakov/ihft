#include <logger/logger.h>

/*

UTC 2023-07-06 08:58:05.515885 DEBUG [main:1818552] logger_simple.cpp(14):'int main()' Hello world !!!
UTC 2023-07-06 08:58:05.518553  INFO [main:1818552] logger_simple.cpp(15):'int main()' Hello world !!!
UTC 2023-07-06 08:58:05.518573  WARN [main:1818552] logger_simple.cpp(16):'int main()' Hello world !!!
UTC 2023-07-06 08:58:05.518587 ERROR [main:1818552] logger_simple.cpp(17):'int main()' Hello world !!!

*/

int main()
{
    IHFT_LOG_DEBUG("Hello {} !!!", "world");
    IHFT_LOG_INFO("Hello {} !!!", "world");
    IHFT_LOG_WARN("Hello {} !!!", "world");
    IHFT_LOG_ERROR("Hello {} !!!", "world");

    return 0;
}
