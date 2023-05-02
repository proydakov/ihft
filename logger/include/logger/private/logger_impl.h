#pragma once

#include <types/box.h>

#include <logger/logger_event.h>

#include <memory/stream_fixed_pool_allocator.h>

#include <channel/channel_factory.h>
#include <channel/one2one_seqnum_stream_object_queue.h>

using event_t = ihft::types::box<ihft::logger::logger_event>;
using alloc_t = ihft::memory::stream_fixed_pool_allocator<ihft::logger::logger_event>;
using queue_t = ihft::channel::one2one_seqnum_stream_object_queue<event_t, alloc_t>;
