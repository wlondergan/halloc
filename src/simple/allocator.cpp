#include "allocator.hpp"
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>


namespace simple_alloc {

    static Allocator a;

    void* alloc(std::size_t size) {
        return a.alloc(size);
    }

    void free(void* block) {
        a.free(block);
    }

    Allocator::~Allocator() {
        if (heap_base == nullptr) {
            return;
        }

        brk(heap_base); // give back the break point.
    }

    void* Allocator::alloc(std::size_t size) {
        size = align(size);

        if (Block* b = find_free_block(size)) {
            return b + sizeof(Block);
        }

        Block* b = request_block(size);
        b->size = size;
        if (this->heap_base == nullptr) {
            this->heap_base = b;
        }
        
        return b + sizeof(Block);
    }

    void Allocator::free(void* block) {

        Block* b = (Block*) (block - sizeof(Block));

        if (this->heap_start == nullptr) {
            heap_start = b;
            heap_top = b;
            b->next = nullptr;
        }
        // check the top of the heap for stupid double frees
        if (b == this->heap_top) {
            crash();
        }

        this->heap_top->next = b;
        b->next = nullptr;
        this->heap_top = b;
    }

    Block* get_block_from_data(void* data) {
        return (Block*) (data + sizeof(Block));
    }

    inline std::size_t align(std::size_t n) {
        return (n + sizeof(word) - 1) & ~(sizeof(word) - 1);
    }

    inline std::size_t alloc_size(std::size_t size) {
        return (size + sizeof(Block));
    }

    inline void crash() {
        raise(SIGSEGV);
    }

    // Allocation uses sbrk-only allocation. More complex implementations
    // can use mmap to get full pages or get bigger blocks of memory.
    Block* request_block(std::size_t size) {
        Block* block = (Block*) sbrk(0);
        if (sbrk(alloc_size(size)) == (void*)(-1)) {
            return nullptr;
        }
        return block;
    }

    Block* Allocator::find_free_block(std::size_t size) {
        Block* b = heap_start;
        
        while (b != nullptr) {
            if (b->size < size) {
                if (can_combine(b, b->next)) {
                    b = combine(b, b->next);
                    return b;
                } else {
                    b = b->next;
                }
            } else if (b->size > size && can_split(b, size)) {
                b = split(b, size);
                return b;
            } else {// just put allocate this block.
                return b;
            } 
        }

        return nullptr;
    }

    inline bool can_combine(Block* a, Block* b) {
        return (a + sizeof(Block) + a->size) == b; // the two segments of memory are contiguous
    }

    inline bool can_split(Block* a, size_t size) {
        return (a->size - sizeof(Block) - size >= sizeof(word));
    }

    Block* split(Block* block, std::size_t size) {
        Block* b = block + sizeof(Block) + size;
        b->next = block->next;
        block->next = b;
        b->size = block->size - sizeof(Block) - size; // the amount of memory we lose
        block->size = size;
        return block;
    }

    Block* combine(Block* b1, Block* b2) {
        b1->next = b2->next;
        b1->size = b1->size + sizeof(Block) + b2->size;
        return b1;
    }
}