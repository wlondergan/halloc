PROGRAMS=simple

CXXFLAGS=-std=c++11 -O0 -fno-inline -g -Wall -Wextra -Wpedantic -pedantic-errors
LDFLAGS=-std=c++11

all: $(PROGRAMS)

simple: src/simple/test.o src/simple/allocator.o
	g++ $(LDFLAGS) $^ -o $@

%.o: %.cpp *.hpp Makefile
	g++ $(CXXFLAGS) -c $< -o $@
	
clean:
	rm -rf test.log *~ .vscode/*~ *.o *.dSYM $(PROGRAMS) *.gcda *.gcno *.gcov callgrind.out.*