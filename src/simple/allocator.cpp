#include "allocator.hpp"
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>


namespace simple_alloc {

    static Allocator a;

    void* malloc(std::size_t size) {
        return a.malloc(size);
    }

    void free(void* block) {
        a.free(block);
    }

    Allocator::~Allocator() {
        if (heap_base == nullptr) {
            return;
        }
        //brk(heap_base); // give back the break point.
        //heap_base = nullptr;
        //heap_top = nullptr;
    }

    void* Allocator::malloc(std::size_t size) {
        size = align(size);

        if (Block* b = find_free_block(size)) {
            return b + 1;
        }

        Block* b = request_block(size);
        b->size = size;
        if (this->heap_base == nullptr) {
            this->heap_base = b;
        }
        
        return b + 1; // add the size of a block
    }

    void Allocator::free(void* block) {

        Block* b = get_block_from_data(block);
        b->free = true;
    }

    Block* Allocator::find_free_block(std::size_t size) {
        Block* b = heap_base;
        
        while (b != nullptr) {
            if (b->free) {
                if (b->size < size) {
                    if (can_combine(b, b->next) && b->size + sizeof(Block) + b->next->size >= size) {
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
            b = b->next;
        }

        return nullptr;
    }

    namespace {
        Block* get_block_from_data(void* data) {
            return ((Block*) data) - 1; // go back one Block size.
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

        inline bool can_combine(Block* a, Block* b) {
            if (b == nullptr) {
                return false;
            }
            return ((word*) (a + 1)) + (a->size) == (word*)b && a->free && b->free; // add the number of words that compose the block's data.
        }

        inline bool can_split(Block* a, size_t size) {
            return a->free && (a->size - sizeof(Block) - size >= sizeof(word));
        }

        Block* split(Block* block, std::size_t size) {
            Block* b = (Block*) (((word*)(block + 1)) + (size));
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
    
}