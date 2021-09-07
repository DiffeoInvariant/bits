CXX ?= clang++
CXXFLAGS ?= -std=c++20 -O3 -g -stdlib=libc++
ASANFLAGS= -fsanitize=address -fno-omit-frame-pointer 

