#include <MockSerial.h>

namespace MockSerial {
  MockSerial::MockSerial() {
    
  }

  void MockSerial::begin(long b) {
    MockSerial::baud = b;
  }

  void MockSerial::println(char *out) {
    std::cout << out << std::endl;
  }

  void MockSerial::println(std::string out) {
    std::cout << out << std::endl;
  }

  void MockSerial::println(float out) {

  }

  void MockSerial::print(std::string out) {

  }

  void MockSerial::print(float out) {

  }

  void MockSerial::flush() {

  }
}

MockSerial::MockSerial Serial;
