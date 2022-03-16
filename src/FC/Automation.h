#pragma once

#include "Comms.h"
#include "BlackBox.h"
#include "Apogee.h"

namespace Automation {
    void init();
    void configureLaunch(Comms::Packet packet);
}