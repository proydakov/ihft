#pragma once

#include <logger/logger_event.h>
#include <logger/logger_client.h>

#define IHFT_LOG_IMPL(level, pattern, ...) \
do \
{ \
if (auto client = ::ihft::logger::logger_client::get_this_thread_client(); client) { \
    auto slab = client->active_event_slab(); \
    auto event = std::construct_at(slab, pattern, __VA_ARGS__); \
    event->set_log_point_info( level, std::chrono::system_clock::now(), source_location_current( ) ); \
    client->try_log_event(event); \
} \
} \
while(0);

#define IHFT_LOG_DEBUG(pattern, ...) IHFT_LOG_IMPL(::ihft::logger::log_level::DEBUG, pattern, __VA_ARGS__)
#define IHFT_LOG_INFO(pattern, ...) IHFT_LOG_IMPL(::ihft::logger::log_level::INFO, pattern, __VA_ARGS__)
#define IHFT_LOG_WARN(pattern, ...) IHFT_LOG_IMPL(::ihft::logger::log_level::WARN, pattern, __VA_ARGS__)
#define IHFT_LOG_ERROR(pattern, ...) IHFT_LOG_IMPL(::ihft::logger::log_level::ERROR, pattern, __VA_ARGS__)
