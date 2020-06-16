#pragma once

namespace ihft
{
    class platform
    {
        public:
            // Change current thread name. Could be useful with htop
            // The name can be up to 16 bytes long, including the terminating null byte.
            // (If the length of the string, including the terminating null byte,
            // exceeds 16 bytes, the string is silently truncated.)
            static void set_current_thread_name(const char* name);
    };
}
