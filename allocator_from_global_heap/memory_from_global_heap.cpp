#include "memory_from_global_heap.h"

void *memory_from_global_heap::allocate(size_t target_size) const {
    this->trace_with_guard("memory_from_global_heap::allocate method execution started");

    try
    {
        auto *block_pointer = ::operator new(target_size + sizeof(size_t));
        *reinterpret_cast<size_t *>(block_pointer) = target_size;

        this->information_with_guard("memory block with _size = " + std::to_string(target_size) + " was allocated successfully")
            ->debug_with_guard("Allocated block address: " + address_to_hex(block_pointer))
            ->trace_with_guard("memory_from_global_heap::allocate method execution finished");

        return reinterpret_cast<void *>(reinterpret_cast<size_t *>(block_pointer) + 1);
    }
    catch (std::bad_alloc const &) {
        this->error_with_guard("memory block with _size = " + std::to_string(target_size) + " cannot be allocated")
            ->trace_with_guard("memory_from_global_heap::allocate method execution finished");
        throw memory::memory_exception("A block cannot be allocated in global heap allocator");
    }
}

void memory_from_global_heap::deallocate(const void *const target_to_dealloc) const {
    this->trace_with_guard("memory_from_global_heap::deallocate method execution started");

    dump_occupied_block_before_deallocate(const_cast<void *>(target_to_dealloc));
    void * tmp = reinterpret_cast<size_t *>(const_cast<void *>(target_to_dealloc)) - 1;

    std::string address = address_to_hex(tmp);

    ::operator delete(tmp);

    this->information_with_guard("memory block with address: " + address + " was deallocated successfully")
        ->trace_with_guard("memory_from_global_heap::deallocate method execution finished");
}

// ptr to metadata start
size_t memory_from_global_heap::get_size_of_occupied_block_pool(void *const occupied_block) const {
    return *(reinterpret_cast<size_t *>(occupied_block) - 1);
}

memory_from_global_heap::memory_from_global_heap(logger * gh_allocator_logger)
{
    try {
        _ptr_to_allocator_metadata = ::operator new(sizeof(logger *));
    }
    catch (std::bad_alloc const &ex) {
        throw memory::memory_exception("An object of global heap allocator cannot be constructed");
    }
    *get_ptr_logger_of_allocator() = gh_allocator_logger;
    this->trace_with_guard("An object of global heap allocator was constructed");
}

memory_from_global_heap::~memory_from_global_heap() {
    this->trace_with_guard("An object of global heap allocator was destructed");
    ::operator delete(_ptr_to_allocator_metadata);
}

logger **memory_from_global_heap::get_ptr_logger_of_allocator() const {
    return reinterpret_cast<logger **>(_ptr_to_allocator_metadata);
}

logger *memory_from_global_heap::get_logger() const noexcept {
    return *get_ptr_logger_of_allocator();
}
