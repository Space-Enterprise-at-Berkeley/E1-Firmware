#include <FakeSerial.h>

FakeSerial::FakeSerial() {}

void FakeSerial::begin(long b) {
  baud = b;
}

void FakeSerial::println(char *out) {
  cout << out << std::endl;
}

void FakeSerial::println(string out) {
  cout << out << std::endl;
}

void FakeSerial::println(uint32_t out) {
  cout << to_string(out) << endl;
}

void FakeSerial::println(float out) {
  cout << to_string(out) << endl;
}
