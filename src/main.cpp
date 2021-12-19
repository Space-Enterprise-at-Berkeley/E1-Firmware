#include <Task.h>
#include <Comms.h>
#include <Ducers.h>
#include <Power.h>
#include <Valves.h>
#include <HAL.h>
#include <Thermocouples.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

Task taskTable[] = {
  // ducers
  {Ducers::ptSample, 0},
  {Power::powerSample, 0},
  {Valves::check_currents, 0}
};

#define TASK_COUNT (sizeof(taskTable) / sizeof (struct Task))

int lowerI2CSpeed(TwoWire *theWire) {
  // Need to lower the I2C clock frequency
  /* START SET I2C CLOCK FREQ */
  #define CLOCK_STRETCH_TIMEOUT 15000
  IMXRT_LPI2C_t *port;
  if (theWire == &Wire) {
    port = &IMXRT_LPI2C1;
  } else if (theWire == &Wire1) {
    port = &IMXRT_LPI2C3;
  } else if (theWire == &Wire2) {
    port = &IMXRT_LPI2C4;
  } else {
    Serial.println("ERROR: UNRECOGNIZED WIRE OBJECT. MUST BE Wire, Wire1, or Wire2");
    return -1;
  }
  port->MCCR0 = LPI2C_MCCR0_CLKHI(55) | LPI2C_MCCR0_CLKLO(59) |
    LPI2C_MCCR0_DATAVD(25) | LPI2C_MCCR0_SETHOLD(40);
  port->MCFGR1 = LPI2C_MCFGR1_PRESCALE(2);
  port->MCFGR2 = LPI2C_MCFGR2_FILTSDA(5) | LPI2C_MCFGR2_FILTSCL(5) |
    LPI2C_MCFGR2_BUSIDLE(3000); // idle timeout 250 us
  port->MCFGR3 = LPI2C_MCFGR3_PINLOW(CLOCK_STRETCH_TIMEOUT * 12 / 256 + 1);
  port->MCCR1 = port->MCCR0;
  port->MCFGR0 = 0;
  port->MFCR = LPI2C_MFCR_RXWATER(1) | LPI2C_MFCR_TXWATER(1);
  port->MCR = LPI2C_MCR_MEN;
  /* END SET I2C CLOCK FREQ */
  return 0;
}

int main() {
  // hardware setup
  Serial.begin(115200);
  while(!Serial) {} // wait for user to open serial port (debugging only)

  TwoWire* I2CBus = &Wire;
  TwoWire* I2CBus1 = &Wire1;
  I2CBus->begin();
  lowerI2CSpeed(I2CBus);
  I2CBus1->begin();

  HAL::initHAL(I2CBus, I2CBus1);
  Ducers::initDucers();
  Power::initPower();
  Valves::initValves();
  Thermocouples::initThermocouples(I2CBus);
  
  while(1) {
    uint32_t ticks = micros(); // current time in microseconds
    for(uint32_t i = 0; i < TASK_COUNT; i++) { // for each task, execute if next time >= current time
      if (ticks >= taskTable[i].nexttime)
        taskTable[i].nexttime = ticks + taskTable[i].taskCall();
    }
    if (Comms::packetWaiting())
      Comms::processPackets();
  }
  return 0;
}
