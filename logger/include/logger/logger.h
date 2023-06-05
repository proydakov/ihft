#pragma once

#include <logger/logger_event.h>
#include <logger/logger_client.h>

#define IHFT_LOG_INFO(pattern, ...) \
if(auto client = ::ihft::logger::logger_client::get_this_thread_client(); client) { \
    /* this should be the second function call */ \
    auto slab = client->active_event_slab(); \
    auto event = std::construct_at(slab, pattern, __VA_ARGS__); \
    event->set_source_location( source_location_current( ) ); \
    client->try_log_event(event); \
}
