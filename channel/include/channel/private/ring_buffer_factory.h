#pragma once

#include <memory>

namespace ihft::channel
{

class ring_buffer_factory
{
public:
    // RA - region allocator type. Used for ring buffer allocation
    template<typename RA>
    static std::shared_ptr<typename RA::value_type> make(std::size_t n, RA region_allocator)
    {
        using bucket_type = typename RA::value_type;

        auto reg_allocator(std::move(region_allocator));
        auto ptr = reg_allocator.allocate(n);
        std::uninitialized_value_construct_n(ptr, n);

        std::shared_ptr<bucket_type> buffer(ptr, [
            r_allocator = std::move(reg_allocator),
            size = n
        ](bucket_type* ptr) mutable {
            std::destroy_n(ptr, size);
            r_allocator.deallocate(ptr, size);
        });

        return buffer;
    }

    template<typename CA, typename CD, typename RA>
    static std::shared_ptr<typename RA::value_type> make(std::size_t n, std::unique_ptr<CA, CD> content_allocator, RA region_allocator = RA())
    {
        using bucket_type = typename RA::value_type;

        auto reg_allocator(std::move(region_allocator));
        auto ptr = reg_allocator.allocate(n);
        std::uninitialized_value_construct_n(ptr, n);

        std::shared_ptr<bucket_type> buffer(ptr, [
            r_allocator = std::move(reg_allocator),
            size = n,
            c_allocator = content_allocator.release(),
            deleter = content_allocator.get_deleter()
        ](bucket_type* ptr) mutable {
            std::destroy_n(ptr, size);
            r_allocator.deallocate(ptr, size);
            // data removed. now we ready to cleanup allocator memory
            deleter(c_allocator);
        });

        return buffer;
    }
};

}
