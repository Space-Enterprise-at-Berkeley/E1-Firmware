#pragma once
#include <iostream>
#include <string>


namespace MockSerial {
  class MockSerial
  {
    private:
      long baud;
    public:
      MockSerial();
      void begin(long b);
      void println(char *out);
      void println(std::string out);
      void println(float out);

      void print(std::string out);
      void print(float out);
      void flush();
  };
}

extern MockSerial::MockSerial Serial;
