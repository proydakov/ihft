#pragma once

#include <cstddef>
#include <cstring>
#include <string_view>

namespace ihft::logger
{

//
// This is helper class. It works together with logger_contract.
// It represents a lightweight wrapper on extra memory in logger slab.
// Also it provides several helpers to place origin types in slab memory region.
//
// @todo : support other string_view types
//
// std::wstring_view   (C++17) std::basic_string_view<wchar_t>
// std::u16string_view (C++17) std::basic_string_view<char16_t>
// std::u32string_view (C++17) std::basic_string_view<char32_t>
// std::u8string_view  (C++20) std::basic_string_view<char8_t>
//

struct logger_extra_data final
{
    logger_extra_data(char* buffer, size_t size)
        : m_buffer(buffer)
        , m_size(size)
    {
    }

    std::string_view place(std::string_view origin)
    {
        if (is_enought(origin.size()))
        {
            std::string_view data(m_buffer, origin.size());
            std::memcpy(m_buffer, origin.data(), origin.size());
            seek_buff(origin.size());
            return data;
        }
        else
        {
            return "<truncated>";
        }
    }

private:
    bool is_enought(size_t size) const
    {
        return size <= m_size;
    }

    void seek_buff(size_t size)
    {
        m_buffer += size;
        m_size -= size;
    }

private:
    char* m_buffer;
    size_t m_size;
};

}
