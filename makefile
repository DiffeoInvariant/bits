CXX=clang++
CXXFLAGS=-std=c++20 -O3 -g -fsanitize=address -fno-omit-frame-pointer

tests: test

test:
	make -C logger/tests
