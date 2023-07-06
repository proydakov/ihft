#pragma once

#include <logger/logger_event.h>
#include <logger/logger_client.h>

consteval size_t compiletime_placeholders_count(std::string_view view, std::string_view pattern)
{
    size_t res{}, pos{};
    while(pos = view.find(pattern, pos), pos != std::string_view::npos)
    {
        res += 1;
        pos += pattern.size();
    }
    return res;
}

template<typename ... Args>
consteval size_t compiletime_args_count(Args&& ... args)
{
    return sizeof...(args);
}

#define IHFT_LOG_IMPL(level, pattern, ...) \
do \
{ \
if (auto client = ::ihft::logger::logger_client::get_this_thread_client(); client) { \
    static_assert(compiletime_placeholders_count(pattern, "{}") == compiletime_args_count(__VA_ARGS__)); \
    auto slab = client->active_event_slab(); \
    auto event = std::construct_at(slab, pattern, __VA_ARGS__); \
    event->set_log_point_source_info( level, std::chrono::system_clock::now(), source_location_current( ) ); \
    client->try_log_event(event); \
} \
} \
while(0);

#define IHFT_LOG_DEBUG(pattern, ...) IHFT_LOG_IMPL(::ihft::logger::log_level::DEBUG, pattern, __VA_ARGS__)
#define IHFT_LOG_INFO(pattern, ...) IHFT_LOG_IMPL(::ihft::logger::log_level::INFO, pattern, __VA_ARGS__)
#define IHFT_LOG_WARN(pattern, ...) IHFT_LOG_IMPL(::ihft::logger::log_level::WARN, pattern, __VA_ARGS__)
#define IHFT_LOG_ERROR(pattern, ...) IHFT_LOG_IMPL(::ihft::logger::log_level::ERROR, pattern, __VA_ARGS__)
