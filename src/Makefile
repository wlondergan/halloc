PROGRAMS=simple

CXXFLAGS=-std=c++11 -O0 -fno-inline -g -Wall -Wextra -Wpedantic -Werror -pedantic-errors
LDFLAGS=-std=c++11

all: $(PROGRAMS)

simple: simple/test.o simple/allocator.o
	g++ $(LDFLAGS) $^ -o $@

%.o: %.cpp *.hpp Makefile
	g++ $(CXXFLAGS) -c $< -o $@