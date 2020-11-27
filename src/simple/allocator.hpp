#pragma once
#include <cstdint>

typedef std::intptr_t word;

namespace simple_alloc {

    const std::size_t MAX_SIZE = 4096 * sizeof(word);

    void* alloc(std::size_t size);

    void free(void* block);

    class Allocator {
        public:
            Allocator(): heap_start(nullptr), heap_top(nullptr) 
                {}

            ~Allocator();

            void* alloc(std::size_t size);

            void free(void* block);
        private:
            Block* heap_base;
            Block* heap_start;
            Block* heap_top;
            Block* find_free_block(std::size_t size);
    };

    struct Block { // For use in a single size list.
        Block* next;
        std::size_t size;
    };
}