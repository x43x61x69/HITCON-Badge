#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_IS31FL3731.h>

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif


/* Constructor */
Adafruit_IS31FL3731::Adafruit_IS31FL3731(uint8_t x, uint8_t y) : Adafruit_GFX(x, y) {
}

Adafruit_IS31FL3731_Wing::Adafruit_IS31FL3731_Wing(void) : Adafruit_IS31FL3731(15, 7) {
}

boolean Adafruit_IS31FL3731::begin(uint8_t addr,uint8_t addr2) {
  Wire.begin();

  _i2caddr = addr;
  _i2caddr2 = addr2;
  _frame = 0;

  // shutdown
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00,_i2caddr);
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00,_i2caddr2);

  delay(10);

  // out of shutdown
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01,_i2caddr);
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01,_i2caddr2);

  // picture mode
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE,_i2caddr);
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE,_i2caddr2);

  displayFrame(_frame);

  // all LEDs on & 0 PWM
  clear(); // set each led to 0 PWM

  for (uint8_t f=0; f<8; f++) {  
    for (uint8_t i=0; i<=0x11; i++){
      writeRegister8(f, i, 0xff,_i2caddr);     // each 8 LEDs on
      writeRegister8(f, i, 0xff,_i2caddr2);     // each 8 LEDs on
    }
  }

  audioSync(false);

  return true;
}

void Adafruit_IS31FL3731::clear(void) {
  // all LEDs on & 0 PWM

  selectBank(_frame);

  for (uint8_t i=0; i<6; i++) {
    Wire.beginTransmission(_i2caddr);
    Wire.write((byte) 0x24+i*24);
    // write 24 bytes at once
    for (uint8_t j=0; j<24; j++) {
      Wire.write((byte) 0);
    }
    Wire.endTransmission();
  }

  for (uint8_t i=0; i<6; i++) {
    Wire.beginTransmission(_i2caddr2);
    Wire.write((byte) 0x24+i*24);
    // write 24 bytes at once
    for (uint8_t j=0; j<24; j++) {
      Wire.write((byte) 0);
    }
    Wire.endTransmission();
  }
}

void Adafruit_IS31FL3731::setLEDPWM(uint16_t lednum, uint8_t pwm, uint8_t bank) {
  if(lednum>=288){
      return;
  }
  if (lednum >= 144){
      writeRegister8(bank, 0x24+lednum-144, pwm,_i2caddr2);
      return;
  }
      writeRegister8(bank, 0x24+lednum, pwm,_i2caddr);
}


void Adafruit_IS31FL3731_Wing::drawPixel(int16_t x, int16_t y, uint16_t color) {
 // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    _swap_int16_t(x, y);
    x = 15 - x - 1;
    break;
  case 2:
    x = 15 - x - 1;
    y = 7 - y - 1;
    break;
  case 3:
    _swap_int16_t(x, y);
    y = 9 - y - 1;
    break;
  }

  // charlie wing is smaller:
  if ((x < 0) || (x >= 16) || (y < 0) || (y >= 7)) return;

  if (x > 7) {
    x=15-x;
    y += 8;
  } else {
    y = 7-y;
  }

  _swap_int16_t(x, y);
 
  if (color > 255) color = 255; // PWM 8bit max

  setLEDPWM(x + y*16, color, _frame);
  return;
}



void Adafruit_IS31FL3731::drawPixel(int16_t x, int16_t y, uint16_t color) {
 // check rotation, move pixel around if necessary
  //Serial.print(x);Serial.print(",");Serial.println(y);
  
  // y = 18 - y - 1;
  switch (getRotation()) {
  case 0:
    //x = 16 - x - 1;
    y = 18 - y - 1;
    break;
  case 1:
    _swap_int16_t(x, y);
    x = 16 - x - 1;
    y = 18 - y - 1;
    break;
  case 2:
    x = 16 - x - 1;
    //y = 18 - y - 1;
    break;
  case 3:
    _swap_int16_t(x, y);
    //y = 18 - y - 1;
    break;
  }
  //Serial.print(x);Serial.print(",");Serial.println(y);
  if ((x < 0) || (x >= 16)) return;
  if ((y < 0) || (y >= 18)) return;
  if (color > 255) color = 255; // PWM 8bit max

  setLEDPWM(x + y*16, color, _frame);
  return;
}

void Adafruit_IS31FL3731::setFrame(uint8_t f) {
  _frame = f;
}

void Adafruit_IS31FL3731::displayFrame(uint8_t f) {
  if (f > 7) f = 0;
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, f, _i2caddr);
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, f, _i2caddr2);
}


void Adafruit_IS31FL3731::selectBank(uint8_t b) {
 Wire.beginTransmission(_i2caddr);
 Wire.write((byte)ISSI_COMMANDREGISTER);
 Wire.write(b);
 Wire.endTransmission();


 Wire.beginTransmission(_i2caddr2);
 Wire.write((byte)ISSI_COMMANDREGISTER);
 Wire.write(b);
 Wire.endTransmission();

}

void Adafruit_IS31FL3731::audioSync(boolean sync) {
  if (sync) {
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x1,_i2caddr);
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x1,_i2caddr2);
  } else {
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0,_i2caddr);
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0,_i2caddr2);
  }
}

/*************/
void Adafruit_IS31FL3731::writeRegister8(uint8_t b, uint8_t reg, uint8_t data,uint8_t addr) {
  selectBank(b);

  Wire.beginTransmission(addr);
  Wire.write((byte)reg);
  Wire.write((byte)data);
  Wire.endTransmission();
  //Serial.print("$"); Serial.print(reg, HEX);
  //Serial.print(" = 0x"); Serial.println(data, HEX);
}

uint8_t  Adafruit_IS31FL3731::readRegister8(uint8_t bank, uint8_t reg,uint8_t addr) {
 uint8_t x;

 selectBank(bank);

 Wire.beginTransmission(addr);
 Wire.write((byte)reg);
 Wire.endTransmission();

 Wire.beginTransmission(addr);
 Wire.requestFrom(addr, (byte)1);
 x = Wire.read();
 Wire.endTransmission();

// Serial.print("$"); Serial.print(reg, HEX);
//  Serial.print(": 0x"); Serial.println(x, HEX);

  return x;
}
