#include <logger/logger_client.h>
#include <logger/logger_adapter.h>

#include <logger/private/logger_impl.h>

#include <cassert>

namespace
{
    thread_local ihft::logger::logger_client* tls_client = nullptr;
}

namespace ihft::logger
{

logger_client* logger_client::get_this_thread_client() noexcept
{
    return tls_client;
}

void logger_client::set_this_thread_client(logger_client* ctx) noexcept
{
    tls_client = ctx;
}

logger_client::~logger_client()
{
    set_this_thread_client(nullptr);

    auto ptr = reinterpret_cast<queue_t*>(m_impl);
    std::destroy_at(ptr);
}

logger_event* logger_client::active_event_slab() noexcept
{
    auto ptr = reinterpret_cast<queue_t*>(m_impl);
    auto& allocator = ptr->get_content_allocator();
    return allocator.active_slab();
}

bool logger_client::try_log_event(logger_event* event) noexcept
{
    auto ptr = reinterpret_cast<queue_t*>(m_impl);
    auto& allocator = ptr->get_content_allocator();

    assert(event == allocator.active_slab());

    bool const write = ptr->try_write(types::box<logger::logger_event>(event));

    if (write)
    {
        allocator.seek_to_next_slab();
        if(m_synch.load(std::memory_order_relaxed))
        {
            logger_adapter::dispatch();
        }
    }
    else
    {
        m_lost++;
    }

    return write;
}

}
