#ifndef __STRINGPATCH__
#define __STRINGPATCH__

#include <sstream>

namespace patch {
  template <typename T> 
  std::string to_string(const T& n) {
    ostringstream s;
    s << n;
    return s.str();
  }
}

#endif // end guard for __STRINGPATCH__