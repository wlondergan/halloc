#pragma once
#include <cstdint>

typedef std::intptr_t word;

namespace simple_alloc {

    class Allocator;

    struct Block;

    const std::size_t MAX_SIZE = 4096 * sizeof(word);

    void* malloc(std::size_t size);

    void free(void* block);

    class Allocator {
        public:
            Allocator(): heap_base(nullptr), heap_top(nullptr) 
                {}

            ~Allocator();

            void* malloc(std::size_t size);

            void free(void* block);
        private:
            Block* heap_base;
            Block* heap_top;
            Block* find_free_block(std::size_t size);
    };

    struct Block { // For use in a single size list.
        Block* next;
        std::size_t size;
        bool free;
    };

    namespace {
        Block* get_block_from_data(void* data);
        inline std::size_t alloc_size(std::size_t size);
        inline std::size_t align(std::size_t n);
        inline void crash();
        Block* request_block(std::size_t size);
        inline bool can_combine(Block* a, Block* b);
        inline bool can_split(Block* a, std::size_t size);
        Block* split(Block* block, std::size_t size);
        Block* combine(Block* b1, Block* b2);
    }
}