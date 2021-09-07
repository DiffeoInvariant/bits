#include "logger.h"

int main(int argc, char **argv) {
  bits::basic_logger<char> logger;
  logger.set_level(bits::log_level::INFO)
    .log("the first message",
	 bits::log_level::CRITICAL)
    .log("the second message",
	 bits::log_level::DEBUG)
    .log("the third message",
	 bits::log_level::INFO);
  

  return 0;
}
