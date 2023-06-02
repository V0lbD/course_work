#ifndef MEMORY_HOLDER_H
#define MEMORY_HOLDER_H

#include "memory_base_class.h"

class memory_holder {
public:
    virtual ~memory_holder() noexcept = default;

public:
    void *allocate_with_guard(size_t block_size) const;

    void deallocate_with_guard(void * block_pointer) const;

protected:
    virtual memory *get_memory() const noexcept = 0;
};


#endif //MEMORY_HOLDER_H
