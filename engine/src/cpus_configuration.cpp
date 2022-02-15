#include <engine/cpus_configuration.h>

namespace ihft::engine
{
    cpus_configuration::cpus_configuration(std::map<std::string, unsigned> map)
        : m_name_2_id(std::move(map))
    {
    }

    cpus_configuration::~cpus_configuration() = default;
}
