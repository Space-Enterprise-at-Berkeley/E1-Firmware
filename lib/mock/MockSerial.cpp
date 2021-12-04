#include "lib/mock/MockSerial.h"


namespace MockSerial {
  MockSerial::MockSerial() {
    MockSerial::baud = 0;
  }

  void MockSerial::begin(long b) {
    MockSerial::baud = b;
  }

  void MockSerial::println(char *out) {
    std::cout << out << "\n";
  }

  void MockSerial::println(std::string out) {
    std::cout << out << "\n";
  }

  void MockSerial::println(float out) {
    std::cout << std::to_string(out) << "\n";
  }

  void MockSerial::print(std::string out) {
    std::cout << out << "\n";
  }

  void MockSerial::print(float out) {
    std::cout << std::to_string(out) << "\n";
  }

  void MockSerial::flush() {
    std::cout << "Serial Flushed";
  }
}

MockSerial::MockSerial Serial;
