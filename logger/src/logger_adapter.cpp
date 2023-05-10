#include <logger/logger_adapter.h>
#include <logger/logger_client.h>
#include <logger/logger_listener.h>
#include <logger/private/default_logger_listener.h>

#include <logger/private/logger_impl.h>

#include <platform/platform.h>

#include <mutex>
#include <cstdlib>
#include <sstream>
#include <iostream>

namespace
{

// SFINAE test
template <typename T>
class has_view
{
    template <typename C> static std::true_type test( decltype(&C::view) );
    template <typename C> static std::false_type test( ... );

public:
    enum { value = std::is_same<decltype(test<T>(0)), std::true_type >::value };
};

template<typename S, typename L>
void notify(L& listener, S& stream)
{
    if constexpr(has_view<S>::value)
    {
        // lets go to use a view from (C++20)
        std::string_view view = stream.view();
        listener->notify(view);
    }
    else
    {
        // clang doesn't implement view() method
        std::string str = stream.str();
        listener->notify(str);
    }
}

}

namespace ihft::logger
{

logger_adapter::logger_client_thread_guard::logger_client_thread_guard()
{
    auto client = logger_client::get_this_thread_client();
    if (nullptr == client)
    {
        m_client = logger_adapter::register_logger_client(global_instance);
    }
}

logger_adapter::logger_client_thread_guard::~logger_client_thread_guard() noexcept
{
}

struct logger_adapter::aimpl final
{
    static constexpr size_t QUEUE_SIZE = 4 * 1024;

    static void atexit_handler()
    {
        if (logger_adapter::global_instance)
        {
            delete logger_adapter::global_instance;
            logger_adapter::global_instance = nullptr;
        }
    }

    aimpl()
        : m_cindex(0)
        , listener(std::make_unique<impl::default_logger_listener>())
        , mode(logger_adapter::mode_t::synch)
    {
        unsigned const cpus = platform::trait::get_total_cpus();

        producers.reserve(cpus);
        consumers.reserve(cpus);
    }

    std::mutex mutex;

    size_t m_cindex;

    std::vector<queue_t::reader_type> consumers;
    std::ostringstream sstream;
    std::unique_ptr<logger_listener> listener;

    // rare usage
    logger_adapter::mode_t mode;
    std::vector<std::shared_ptr<logger_client>> producers;
};

logger_adapter* logger_adapter::create_instance()
{
    const int result = std::atexit(aimpl::atexit_handler);

    if (result == 0)
    {
        return new logger_adapter();
    }
    else
    {
        return nullptr;
    }
}

std::shared_ptr<logger_client> logger_adapter::register_logger_client(logger_adapter* adapter)
{
    if (adapter)
    {
        auto opt = channel::channel_factory::make<queue_t>(aimpl::QUEUE_SIZE, 1, std::make_unique<alloc_t>(aimpl::QUEUE_SIZE));
        if (opt)
        {
            auto& impl = *adapter->m_impl;
            auto& [producer, consumers] = *opt;

            std::unique_lock lock(impl.mutex);

            impl.producers.emplace_back(new logger_client(std::move(producer), impl.mode == mode_t::synch));
            impl.consumers.emplace_back(std::move(consumers.front()));

            return impl.producers.back();
        }
    }

    return {};
}

logger_adapter::logger_adapter()
    : m_impl(std::make_unique<aimpl>())
{
    register_logger_client(this);
}

logger_adapter::~logger_adapter()
{
}

bool logger_adapter::dispatch() noexcept
{
    if (global_instance)
    {
        auto& impl = *global_instance->m_impl;
        std::unique_lock lock(impl.mutex);

        size_t i = 0;

        for(; i < impl.consumers.size(); i++)
        {
            size_t const cindex = (impl.m_cindex + i) % impl.consumers.size();
            queue_t::reader_type& source = impl.consumers[cindex];

            if (auto opt = source.try_read(); opt)
            {
                logger::logger_event const& event_cref = opt->get_event();

                td::ostringstream& stream = impl.sstream;
                /// @todo : reset flags, width, precession
                stream.str(std::string{});

                event_cref.print_args_to(stream);
                notify(impl.listener, stream);

                break;
            }
        }

        bool const flush = i < impl.consumers.size();
        if (flush)
        {
            impl.listener->flush();
            impl.m_cindex += i + 1;
            impl.m_cindex %= impl.consumers.size();
        }

        std::cout << "consumers: " << impl.consumers.size() << std::endl;

        return flush;
    }

    return false;
}

void logger_adapter::change_mode(mode_t newmode) noexcept
{
    if (global_instance)
    {
        auto& impl = *global_instance->m_impl;
        std::unique_lock lock(impl.mutex);

        if (impl.mode != newmode)
        {
            impl.mode = newmode;

            for(auto& p : impl.producers)
            {
                p->set_mode(newmode == mode_t::synch);
            }
        }
    }
}

void logger_adapter::replace_listener(std::unique_ptr<logger_listener> listener) noexcept
{
    if (global_instance)
    {
        auto& impl = *global_instance->m_impl;
        std::unique_lock lock(impl.mutex);

        impl.listener = std::move(listener);
    }
}

}
