
include ../../makefile

default: tests

tests: test_in_memory_storage test_logger

test_in_memory_storage: test_in_memory_storage.cc
	clang++ $(CXXFLAGS) $(ASANFLAGS) -I../ $^ -o test_in_memory_storage


test_logger: test_logger.cc
	clang++ $(CXXFLAGS) $(ASANFLAGS) -I../ $^ -o test_logger
