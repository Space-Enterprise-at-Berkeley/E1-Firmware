#include "Util.h"

namespace Util {
    int secondsToMillis(int seconds) {
        return seconds * 1000;
    }

    int secondsToMicros(int seconds) {
        return seconds * 1000 * 1000;
    }

    int millisToMicros(int millis) {
        return millis * 1000;
    }
}