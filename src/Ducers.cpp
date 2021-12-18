#include <Ducers.h>

namespace Ducers {
    uint32_t ptUpdatePeriod = 1 * 1000;

    void initDucers() {
        
    }

    uint32_t ptSample() {
        // read from all 6 PTs in sequence
        // store the latest PT values into Ducers namespace
        // emit a packet with data
        // return the next execution time
        return ptUpdatePeriod;
    }
};
