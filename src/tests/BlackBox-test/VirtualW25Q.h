/*
  W25Q16.h - Arduino library for communicating with the Winbond W25Q16 Serial Flash.
  Created by Derek Evans, July 17, 2016.
*/

#ifndef VirtualW25Q_h
#define VirtualW25Q_h

#include "Arduino.h"
#include "SPI.h"

class VirtualW25Q
{
  public:
  const int numPages = 65536;
  const int pageSize = 256;
  void init(int FLASH_SS);
  void write(unsigned int page, byte pageAddress, byte val);
  byte read(unsigned int page, byte pageAddress);
  void initStreamWrite(unsigned int page, byte pageAddress);
  void streamWrite(byte val);
  void closeStreamWrite();
  void initStreamRead(unsigned int page, byte pageAddress);
  byte streamRead();
  void closeStreamRead();
  void powerDown();
  void releasePowerDown();
  void chipErase();
  byte manufacturerID();
};

#endif
