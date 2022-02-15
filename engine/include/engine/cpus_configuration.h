#pragma once

#include <types/result.h>

#include <map>
#include <string>

namespace ihft::misc
{
class config_helper;
}

namespace ihft::engine
{

class cpus_configuration
{
public:
    using cconfiguration_result_t = ihft::types::result<cpus_configuration, std::string>;

    template<typename platform, typename config>
    static cconfiguration_result_t parse(config const& cfg)
    {
        std::map<std::string, unsigned> map;
        cfg.enumerate_integer("engine.cpu", [&](std::string_view key, std::int64_t val)  mutable {
            map[std::string(key)] = static_cast<unsigned>(val);
        });
        return parse<platform>(std::move(map));
    }

    template<typename platform>
    static cconfiguration_result_t parse(std::map<std::string, unsigned> cfg)
    {
        const std::string prefix = "Invalid configuration. ";

        if(cfg.empty())
        {
            return prefix + "Empty configuration doesn't allowed, please specify at least one logical cpu.";
        }

        auto const total_cpus = platform::get_total_cpus();

        std::map<unsigned, std::string> uniq;

        for(auto const& [name, id] : cfg)
        {
            if(id >= total_cpus)
            {
                return prefix + "Core: " + std::to_string(id) + " is bigger than total_cpus: " + std::to_string(total_cpus);
            }

            if(!platform::get_cpu_isolation_status(id))
            {
                return prefix + "Core: " + std::to_string(id) + " doesn't have isolation.";
            }

            if(!platform::get_cpu_nohz_full_status(id))
            {
                return prefix + "Core: " + std::to_string(id) + " doesn't have nohz_full.";
            }

            if(!platform::get_cpu_rcu_nocbs_status(id))
            {
                return prefix + "Core: " + std::to_string(id) + " doesn't have rcu_nocbs.";
            }

            auto const [it, res] = uniq.insert(std::make_pair(id, name));
            if(!res)
            {
                return prefix + "Logic cpus: [" + it->second + ", " + name + "] use same core: " + std::to_string(id);
            }
        }

        return cpus_configuration(std::move(cfg));
    }

    ~cpus_configuration();

    auto const& get_name_2_cpu() const noexcept
    {
        return m_name_2_id;
    }

    template<typename S>
    friend S& operator<<(S& os, const cpus_configuration& cfg)
    {
        os << "{";
        for(auto const& [k, v] : cfg.m_name_2_id)
        {
            os << "[" << k << "] -> " << v << "\n";
        }
        os << "}";

        return os;
    }

private:
    cpus_configuration(std::map<std::string, unsigned>);

private:
    std::map<std::string, unsigned> m_name_2_id;
};

}
