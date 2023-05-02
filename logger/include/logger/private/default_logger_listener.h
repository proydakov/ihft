#pragma once

#include <logger/logger_listener.h>

namespace ihft::logger::impl
{
    class default_logger_listener final : public logger_listener
    {
    public:
        void notify(std::string_view) override;

        void flush() override;
    };
}
