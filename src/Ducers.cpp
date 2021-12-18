#include <Ducers.h>

namespace Ducers {
    uint32_t ptUpdatePeriod = 100 * 1000;

    void initDucers() {

    }

    uint32_t ptSample() {
        // read from all 6 PTs in sequence
        HAL::adc1.read(0);
        // store the latest PT values into Ducers namespace
        // emit a packet with data
        Serial.println("yeet");
        // return the next execution time
        return ptUpdatePeriod;
    }
};
