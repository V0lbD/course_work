#ifndef MEMORY_BASE_CLASS_H
#define MEMORY_BASE_CLASS_H

#include "../logger/logger_builder.h"
#include "../logger/logger_holder.h"

/* Common structure for allocators:
 * size_t          -- size of allocator pool
 * logger*
 * memory*         -- parent allocator (if present)
 * allocation_mode -- first/best/worst
 * void *          -- ptr to (available/occupied block from pool)
 *
 * Common for all blocks:
 *  - if size_t size is used, it is a size of a whole block including service part
 *
 * */

class memory : protected logger_holder
{
public:
    enum Allocation_strategy {
        first_fit,
        best_fit,
        worst_fit,
    };

    class memory_exception final : public std::exception
    {
    private:
        std::string _message;

    public:
        explicit memory_exception(std::string const & message)
                : _message(message)
        {

        }

        char const * what() const noexcept override
        {
            return _message.c_str();
        }

    };

protected:
    void *_ptr_to_allocator_metadata;
    void dump_occupied_block_before_deallocate(void *const current_block_address) const;

#pragma region Allocator properties
    [[nodiscard]] virtual size_t get_allocator_service_block_size() const;
    [[nodiscard]] virtual size_t* get_ptr_size_of_allocator_pool() const;
    [[nodiscard]] virtual logger** get_ptr_logger_of_allocator() const;
    [[nodiscard]] virtual memory** get_ptr_to_ptr_parent_allocator() const;
    [[nodiscard]] virtual Allocation_strategy* get_ptr_allocation_mode() const;
    [[nodiscard]] virtual void ** get_ptr_to_ptr_to_pool_start() const;
    [[nodiscard]] virtual void * get_ptr_to_allocator_trusted_pool() const;
#pragma endregion

#pragma region Available block methods
    [[nodiscard]] virtual void *get_first_available_block_address() const; // get the address of the first available block in allocator
    [[nodiscard]] virtual void ** get_first_available_block_address_address() const;   // get an address field of the first block available in allocator

    [[nodiscard]] virtual size_t get_available_block_service_block_size() const;
    virtual size_t get_available_block_size(void * memory_block) const;
    virtual void * get_next_available_block_address(void * memory_block) const;
#pragma endregion

#pragma region Occupied block methods
    [[nodiscard]] virtual void ** get_first_occupied_block_address_address() const;

    [[nodiscard]] virtual size_t get_occupied_block_service_block_size() const;
    virtual size_t get_occupied_block_size(void * memory_block) const;
    virtual size_t get_size_of_occupied_block_pool(void * const occupied_block) const = 0;

    virtual void * get_next_occupied_block_address(void * memory_block) const;
    virtual void * get_previous_occupied_block_address(void * memory_block) const;
#pragma endregion

    std::string address_to_hex(void const * ptr) const;

public:
    virtual ~memory() noexcept = default;

    virtual void *allocate(size_t target_size) const = 0;

    virtual void deallocate(void const * const target_to_dealloc) const = 0;

    friend void * operator+=(memory const &allocator, size_t target_size);
    friend void operator-=(memory const &allocator, void const * const target_to_dealloc);
};

#endif //MEMORY_BASE_CLASS_H
