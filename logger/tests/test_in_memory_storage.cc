#include "logger.h"
#include <iostream>
//#include "types.h"
#include <thread>
template<typename storage>
void test_in_memory_storage(const std::vector<std::string>& entries,
			    const std::vector<bits::log_level>& levels,
			    storage backing)
			    //typename std::enable_if<bits::is_storage<storage>::value>::type backing)
{
  for (auto i=0; i<entries.size(); ++i) {
    backing.write(entries[i], levels[i], storage::clock_type::now());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  std::cout << "repr() returns:\n" << backing.repr() << '\n';
  std::cout << "Iterating over entries using read() getting entries with level DEBUG or greater:\n";
  for (auto i=0; i<backing.size(); ++i) {
    std::cout << backing.read(i, 1, bits::log_level::DEBUG) << '\n';
  }
}

int main(int argc, char **argv)
{
  bits::in_memory_storage backing;
  //std::cout << "bits::is_storage<T> works? " << std::boolalpha << bits::is_storage<decltype(backing)>::value << '\n';
  std::vector<std::string> entries{{"a debug message"}, {"a warning message"}, {"an unset message"}};
  std::vector<bits::log_level> levels{bits::log_level::DEBUG, bits::log_level::WARNING, bits::log_level::NOTSET};
  test_in_memory_storage(entries, levels, backing);
  bits::basic_logger<char> logger;
  return 0;
}
