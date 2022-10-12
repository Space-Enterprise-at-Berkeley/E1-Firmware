#include "Arduino.h"
#include "VirtualW25Q.h"


void W25Q::init(int FLASH_SS)
{

}

byte W25Q::read(unsigned int page, byte pageAddress) {
  digitalWrite(_FLASH_SS, LOW);
  SPI.transfer(READ_DATA);
  SPI.transfer((page >> 8) & 0xFF);
  SPI.transfer((page >> 0) & 0xFF);
  SPI.transfer(pageAddress);
  byte val = SPI.transfer(0);
  digitalWrite(_FLASH_SS, HIGH);
  notBusy();
  return val;
}

void W25Q::write(unsigned int page, byte pageAddress, byte val) {
    writeEnable();
    digitalWrite(_FLASH_SS, LOW);
    SPI.transfer(PAGE_PROGRAM);
    SPI.transfer((page >> 8) & 0xFF);
    SPI.transfer((page >> 0) & 0xFF);
    SPI.transfer(pageAddress);
    SPI.transfer(val);
    digitalWrite(_FLASH_SS, HIGH);
    notBusy();
    writeDisable();
}

void W25Q::initStreamWrite(unsigned int page, byte pageAddress) {
	writeEnable();
    digitalWrite(_FLASH_SS, LOW);
    SPI.transfer(PAGE_PROGRAM);
    SPI.transfer((page >> 8) & 0xFF);
    SPI.transfer((page >> 0) & 0xFF);
    SPI.transfer(pageAddress);
}

void W25Q::streamWrite(byte val) {
	SPI.transfer(val);
}


void W25Q::closeStreamWrite() {
	digitalWrite(_FLASH_SS, HIGH);
    notBusy();
    writeDisable();
}


void W25Q::initStreamRead(unsigned int page, byte pageAddress) {
	digitalWrite(_FLASH_SS, LOW);
  	SPI.transfer(READ_DATA);
  	SPI.transfer((page >> 8) & 0xFF);
  	SPI.transfer((page >> 0) & 0xFF);
  	SPI.transfer(pageAddress);
}


byte W25Q::streamRead() {
	return SPI.transfer(0);
}


void W25Q::closeStreamRead() {
	digitalWrite(_FLASH_SS, HIGH);
    notBusy();
}


void W25Q::powerDown() {
  digitalWrite(_FLASH_SS, LOW);
  SPI.transfer(POWER_DOWN);
  digitalWrite(_FLASH_SS, HIGH);
  notBusy();
}


void W25Q::releasePowerDown() {
  digitalWrite(_FLASH_SS, LOW);
  SPI.transfer(RELEASE_POWER_DOWN);
  digitalWrite(_FLASH_SS, HIGH);
  notBusy();
}


void W25Q::chipErase() {
  writeEnable();
  digitalWrite(_FLASH_SS, LOW);
  SPI.transfer(CHIP_ERASE);
  digitalWrite(_FLASH_SS, HIGH);
  notBusy();
  writeDisable();
}


byte W25Q::manufacturerID() {
  digitalWrite(_FLASH_SS, LOW);
  SPI.transfer(MANUFACTURER_ID);
  SPI.transfer(0);
  SPI.transfer(0);
  SPI.transfer(0);
  byte val = SPI.transfer(0);
  digitalWrite(_FLASH_SS, HIGH);
  notBusy();
  return val;
}


void W25Q::notBusy() {
  digitalWrite(_FLASH_SS, LOW);
  SPI.transfer(READ_STATUS_REGISTER_1);
  while (bitRead(SPI.transfer(0),0) & 1) {
  }
  digitalWrite(_FLASH_SS, HIGH);
}


void W25Q::writeEnable() {
  digitalWrite(_FLASH_SS, LOW);
  SPI.transfer(WRITE_ENABLE);
  digitalWrite(_FLASH_SS, HIGH);
  notBusy();
}


void W25Q::writeDisable() {
  digitalWrite(_FLASH_SS, LOW);
  SPI.transfer(WRITE_DISABLE);
  digitalWrite(_FLASH_SS, HIGH);
  notBusy();
}
