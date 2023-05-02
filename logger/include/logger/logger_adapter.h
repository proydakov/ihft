#pragma once

#include <memory>

namespace ihft::logger
{

///
/// The logger_adapter is main class of logging system.
/// It has two modes: synch and async. Default mode is synch.
///
///
/// The end user interacts with the logging system via thread_local logger_client*.
/// By default the logger_adapter initializes only one logger_client* in main thread.
///
///
/// Let's explore how the logging system works in synch mode on a particular example.
///
/// The initial state:
/// main thread: []
/// LOG_EVENT(...) will put log_event into a this thread queue:
/// main thread: [log_event]
/// Immediately after that, the `logger_adapter_instanse.dispatch()` will be called.
/// main thread: []
///
///
/// Let's explore how the logging system works in async mode on a particular example.
///
/// The user should create a `logger_client_thread_guard` on the stack of each user thread
/// to initialize a dedicated lock-free queue and setup thread_local logger_client pointer.
///
/// main_thread:  [log_event, ....] ----|
/// net_thread:   [log_event, ....] ----|---- logger_thread::dispatch()
/// algo_thread:  [log_event, ....] ----|
///
/// In Asynchronous mode, it calls the `dispatch()` method in a separate thread.
///

class logger_client;
class logger_listener;

class logger_adapter final
{
public:
    //
    // The user should create a guard on the stack of each user thread instead of main.
    //
    class logger_client_thread_guard final
    {
    public:
        logger_client_thread_guard();
        ~logger_client_thread_guard() noexcept;

        logger_client_thread_guard(const logger_client_thread_guard&) = delete;
        logger_client_thread_guard(logger_client_thread_guard&&) noexcept = delete;

    private:
        std::shared_ptr<logger_client> m_client;
    };

public:
    ~logger_adapter();

    //
    // Consume logger_events from readers and process it.
    //
    static void dispatch() noexcept;

    //
    // Changing the operating mode between synchronous and asynchronous.
    //
    enum class mode_t { synch, async };

    static void change_mode(mode_t) noexcept;

    static void replace_listener(std::unique_ptr<logger_listener>) noexcept;

private:
    logger_adapter();

    static logger_adapter* create_instance();

    static std::shared_ptr<logger_client> register_logger_client(logger_adapter*);

    struct aimpl;
    std::unique_ptr<aimpl> m_impl;

private:
    inline static logger_adapter* global_instance = create_instance();
};

}
