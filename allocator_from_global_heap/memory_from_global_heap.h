#ifndef MEMORY_FROM_GLOBAL_HEAP_H
#define MEMORY_FROM_GLOBAL_HEAP_H

#include "../allocator/memory_base_class.h"

class memory_from_global_heap final : public memory
{
    logger** get_ptr_logger_of_allocator() const override;

public:
    explicit memory_from_global_heap(logger * gh_allocator_logger);

    ~memory_from_global_heap() override;

    memory_from_global_heap(memory_from_global_heap const &) = delete;

    memory_from_global_heap& operator=(memory_from_global_heap const&) = delete;

    void *allocate(size_t target_size) const override;

    void deallocate(void const * const target_to_dealloc) const override;

    size_t get_size_of_occupied_block_pool(void * const occupied_block) const override;

private:

    logger *get_logger() const noexcept override;
};

#endif //MEMORY_FROM_GLOBAL_HEAP_H
