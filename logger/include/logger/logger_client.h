#pragma once

#include <atomic>
#include <memory>
#include <type_traits>

namespace ihft::logger
{

struct logger_event;
class logger_adapter;

class logger_client final
{
    static constexpr size_t content_s = 64;

public:
    static logger_client* get_this_thread_client() noexcept;
    ~logger_client();

    logger_client(const logger_client&) = delete;
    logger_client(logger_client&&) noexcept = delete;

    logger_event* active_event_slab() noexcept;
    bool try_log_event(logger_event*) noexcept;

private:
    friend class logger_adapter;

    template<typename T>
    logger_client(T producer, bool synch) noexcept
        : m_synch(synch)
        , m_lost{0}
    {
        static_assert(logger_client::content_s == sizeof(T));
        static_assert(logger_client::content_s == alignof(T));

        m_impl = std::construct_at(reinterpret_cast<T*>(&m_storage), std::move(producer));
        set_this_thread_client(this);
    }

    void set_mode(bool synch)
    {
        m_synch.store(synch);
    }

    static void set_this_thread_client(logger_client*) noexcept;

private:
    std::atomic_bool m_synch;
    std::uint64_t m_lost;
    void* m_impl;
    std::aligned_storage_t<content_s, content_s> m_storage;
};

}
