#include <platform/private/isolation.h>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace
{

template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& vec)
{
    os << "[ ";
    for(auto const& elem : vec)
    {
        os << elem << ' ';
    }
    os << "]";
    return os;
}

struct temp_file
{
    temp_file(std::string path, std::string const& content)
        : m_path(std::move(path))
    {
        std::ofstream file(m_path);
        file << content;
    }

    ~temp_file()
    {
        std::remove(m_path.c_str());
    }

    const char* path() const
    {
        return m_path.c_str();
    }

private:
    std::string m_path;
};

}

int test_impl(temp_file const& file, std::vector<unsigned long> result)
{
    std::vector<unsigned long> data;

    ihft::impl::isolation isolation(file.path());

    for(unsigned long cpu = 0; cpu < 64; cpu++)
    {
        if(isolation.is_isolated(cpu))
        {
            data.push_back(cpu);
        }
    }

    if (data != result)
    {
        std::cerr << "invalid result: " << data << " != " << result << "\n";
        return 1;
    }
    else
    {
        return 0;
    }
}

int test_1()
{
    temp_file file("cmdline_1.txt",
        "BOOT_IMAGE=/boot/vmlinuz-3.16.0-44-generic auto noprompt priority=critical locale=en_US isolcpus=3,5,7 quiet\r\n");

    return test_impl(file, {3,5,7});
}

int test_2()
{
    temp_file file("cmdline_2.txt",
        "BOOT_IMAGE=/boot/vmlinuz-3.16.0-44-generic auto noprompt priority=critical locale=en_US quiet isolcpus=9,17,2\r\n");

    return test_impl(file, {2,9,17});
}

int test_3()
{
    temp_file file("cmdline_3.txt",
        "BOOT_IMAGE=/boot/vmlinuz-3.16.0-44-generic auto noprompt priority=critical quiet isolcpus=8-11,17 locale=en_US\r\n");

    return test_impl(file, {8,9,10,11,17});
}

int main(int, char const*[])
{
    return test_1() + test_2() + test_3();
}
