#include <platform/platform.h>

#include <sched.h>
#include <sys/prctl.h>

namespace ihft
{
    void platform::set_current_thread_name(const char* name)
    {
        prctl(PR_SET_NAME, name, 0, 0, 0);
    }

    void platform::set_current_thread_cpu(unsigned long cpu)
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(cpu, &cpuset);

        sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
    }
}
