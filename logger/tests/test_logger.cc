#include "logger.h"

int main(int argc, char **argv) {
  bits::basic_logger<char> logger;
  logger.set_level(bits::log_level::INFO)
    .set_name("parent")
    .set_persist_all()
    .log("the first message",
	 bits::log_level::CRITICAL)
    .log("the second message",
	 bits::log_level::DEBUG)
    .log("the third message",
	 bits::log_level::INFO);

  auto subl = logger.get_sublogger("child");
  subl.log("a child message",
	   bits::log_level::INFO);

  std::cout << "The log contains (most recent to least):\n";
  for (auto entry = logger.rbegin(); entry != logger.rend(); ++entry) {
    std::cout << logger.format_entry(*entry) << decltype(logger)::backing_type::new_line();
  }

  return 0;
}
