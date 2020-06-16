#include <platform/platform.h>

#include <sys/prctl.h>

namespace ihft
{
    void platform::set_current_thread_name(const char* name)
    {
        prctl(PR_SET_NAME, name, 0, 0, 0);
    }
}
