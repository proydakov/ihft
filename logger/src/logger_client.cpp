#include <logger/logger_client.h>
#include <logger/logger_adapter.h>

#include <logger/private/logger_impl.h>

#include <cassert>
#include <utility>

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

    if (m_pending)
    {
        std::destroy_at(ptr->get_content_allocator().active_slab());
        m_pending = false;
    }

    std::destroy_at(ptr);
}

logger_event* logger_client::active_event_slab() noexcept
{
    auto ptr = reinterpret_cast<queue_t*>(m_impl);
    auto& allocator = ptr->get_content_allocator();

    // A failed try_write() left a prepared event in the active slab. The
    // caller is about to construct a new one, so destroy the old object
    // instead of constructing over a live object.
    if (m_pending)
    {
        std::destroy_at(allocator.active_slab());
        m_pending = false;
    }

    return allocator.active_slab();
}

bool logger_client::try_log_event(logger_event* event) noexcept
{
    auto ptr = reinterpret_cast<queue_t*>(m_impl);
    auto& allocator = ptr->get_content_allocator();

    assert(event == allocator.active_slab());
    event->set_log_point_thread_info(m_thread_id, m_thread_name);

    types::box<logger::logger_event> box(event);
    bool const write = ptr->try_write(std::move(box));
    if (write)
    {
        m_pending = false;
        allocator.seek_to_next_slab();
        if (m_synch.load(std::memory_order_relaxed))
        {
            logger_adapter::dispatch();
        }
    }
    else
    {
        // Keep the prepared event alive so the caller can retry. It is owned by
        // the slab again (not by the queue), and the next active_event_slab()
        // or the destructor will destroy it if it is never written.
        box.release();
        m_pending = true;
        m_lost++;
    }

    return write;
}

}
