#include "allocator.hpp"
#include <iostream>
using namespace simple_alloc;

int main() {
    int* a = (int*) alloc(sizeof(int));
    *a = 5;

    std::cout << a << std::endl;
    std::cout << *a << std::endl;
}