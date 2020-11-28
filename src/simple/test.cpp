#include "allocator.hpp"
#include <iostream>
#include <assert.h>
using namespace std;

struct LL {
    int i;
    LL* next;
    LL(int i, LL* next): i(i), next(next) {}
};

int main() {
    LL head = LL(0, nullptr);
    LL* tail = &head;
    for (int i = 1; i < 10000; i++) {
        tail->next = (LL*) simple_alloc::malloc(sizeof(LL));
        tail->next->i = i;
        tail = tail->next;
    }
    assert(head.next->i == 1);

    LL* prev = &head;
    tail = prev->next;
    for (int i = 0; i < 4999; i++) {
        prev->next = tail->next; // skip every other element.
        simple_alloc::free(tail);
        prev = prev->next;
        tail = prev->next;
    }

    tail = &head;
    for (int i = 0; i < 5000; i++) {
        cout << tail->i << endl;
        tail = tail->next;
    }
}