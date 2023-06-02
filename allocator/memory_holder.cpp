#include "memory_holder.h"

void *memory_holder::allocate_with_guard(size_t block_size) const {
    auto * allocator = get_memory();

    return allocator == nullptr ?
        ::operator new(block_size)
        : allocator->allocate(block_size);
}

void memory_holder::deallocate_with_guard(void *block_pointer) const {
    auto *allocator = get_memory();

    if (allocator == nullptr) {
        ::operator delete(block_pointer);
    } else {
            allocator->deallocate(block_pointer);
    }
 }
