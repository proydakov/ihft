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
        , m_pending{false}
        , m_thread_id{0}
    {
        static_assert(logger_client::content_s == sizeof(T));
        static_assert(logger_client::content_s == alignof(T));

        m_impl = std::construct_at(reinterpret_cast<T*>(&m_storage), std::move(producer));

        // Initialize the whole array: the event header copies the full name
        // buffer, so the unused tail must not be indeterminate.
        constexpr char unknown[] = "unknown";
        static_assert(sizeof(unknown) <= sizeof(m_thread_name));
        for (size_t i = 0; i < sizeof(m_thread_name); i++)
        {
            m_thread_name[i] = (i < sizeof(unknown)) ? unknown[i] : '\0';
        }
        set_this_thread_client(this);
    }

    void set_mode(bool synch, std::memory_order order = std::memory_order_relaxed)
    {
        m_synch.store(synch, order);
    }

    void set_thread_id(long id)
    {
        m_thread_id = id;
    }

    void set_thread_name(const char (&tname)[16])
    {
        static_assert(sizeof(tname) == sizeof(m_thread_name));
        for(size_t i = 0; i < sizeof(tname); i++)
        {
            m_thread_name[i] = tname[i];
        }
    }

    static void set_this_thread_client(logger_client*) noexcept;

private:
    std::atomic_bool m_synch;
    std::uint64_t m_lost;
    bool m_pending;
    long m_thread_id;
    char m_thread_name[16];
    void* m_impl;
    std::aligned_storage_t<content_s, content_s> m_storage;
};

}
