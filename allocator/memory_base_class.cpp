#include "memory_base_class.h"

#pragma region Reach && change allocator properties
size_t* memory::get_ptr_size_of_allocator_pool() const {
    return reinterpret_cast<size_t *>(_ptr_to_allocator_metadata);
}

logger **memory::get_ptr_logger_of_allocator() const {
    return reinterpret_cast<logger **>(get_ptr_size_of_allocator_pool() + 1);
}

memory **memory::get_ptr_to_ptr_parent_allocator() const {
    return reinterpret_cast<memory**>(get_ptr_logger_of_allocator() + 1);
}

memory::Allocation_strategy* memory::get_ptr_allocation_mode() const {
    return reinterpret_cast<memory::Allocation_strategy *>(get_ptr_to_ptr_parent_allocator() + 1);
}

void **memory::get_ptr_to_ptr_to_pool_start() const {
    return reinterpret_cast<void **>(get_ptr_allocation_mode() + 1);
}
#pragma endregion

std::string memory::address_to_hex(const void * ptr) const {
    char address_buf[(sizeof(void const * const) << 3) + 3];
    sprintf(address_buf, "0x%p", ptr);
    return std::string { address_buf };
}

void memory::dump_occupied_block_before_deallocate(void *const current_block_address) const {
    if (*get_ptr_logger_of_allocator() == nullptr) {
        return;
    }

    auto const current_block_size = get_size_of_occupied_block_pool(current_block_address);
    auto const * dump_iterator = reinterpret_cast<unsigned char *>(current_block_address);
    std::string result;

    for (auto i = 0; i < current_block_size; i++) {
        result += std::to_string(static_cast<unsigned short>(*dump_iterator++));

        if (i != current_block_size - 1) {
            result += ' ';
        }
    }

    this->trace_with_guard("Block dump: [" + result + "]");
}

void *operator+=(const memory &allocator, size_t target_size) {
    return allocator.allocate(target_size);
}

void operator-=(const memory &allocator, const void *const target_to_dealloc) {
    allocator.deallocate(target_to_dealloc);
}

#pragma region virtual in-class methods implemention
void *memory::get_first_available_block_address() const {
    throw memory::memory_exception("Method get_first_available_block_address not implemented");
}

void **memory::get_first_available_block_address_address() const {
    throw memory::memory_exception("Method get_first_available_block_address_address not implemented");
}

size_t memory::get_available_block_service_block_size() const {
    throw memory::memory_exception("Method get_available_block_service_block_size not implemented");
}

size_t memory::get_available_block_size(void * memory_block) const
{
    throw memory::memory_exception("Method get_available_block_size not implemented");
}

void * memory::get_next_available_block_address(void * memory_block) const
{
    throw memory::memory_exception("Method get_next_available_block_address not implemented");
}

void ** memory::get_first_occupied_block_address_address() const
{
    throw memory::memory_exception("Method get_first_occupied_block_address_address not implemented");
}

size_t memory::get_occupied_block_service_block_size() const
{
    throw memory::memory_exception("Method get_occupied_block_service_block_size not implemented");
}

size_t memory::get_occupied_block_size(void * memory_block) const
{
    throw memory::memory_exception("Method get_occupied_block_size not implemented");
}

void * memory::get_next_occupied_block_address(void * memory_block) const
{
    throw memory::memory_exception("Method get_next_occupied_block_address not implemented");
}

void * memory::get_previous_occupied_block_address(void * memory_block) const
{
    throw memory::memory_exception("Method get_previous_occupied_block_address not implemented");
}

size_t memory::get_allocator_service_block_size() const {
    throw memory::memory_exception("Method get_allocator_service_block_size not implemented");
}

void *memory::get_ptr_to_allocator_trusted_pool() const {
    throw memory::memory_exception("Method get_ptr_to_allocator_trusted_pool not implemented");
}

#pragma endregion
