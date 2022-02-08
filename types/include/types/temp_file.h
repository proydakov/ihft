#pragma once

#include <string>
#include <fstream>
#include <string_view>

namespace ihft::types
{

class temp_file
{
public:
    temp_file(std::string fname, std::string_view content)
        : m_fname(std::move(fname))
    {
        std::ofstream output(m_fname);
        output << content;
    }

    ~temp_file()
    {
        std::remove(m_fname.c_str());
    }

    std::string const& fpath() const noexcept
    {
        return m_fname;
    }

private:
    std::string m_fname;
};

}
