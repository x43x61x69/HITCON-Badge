#ifndef _ADAFRUIT_IS31FL3731_H_
#define _ADAFRUIT_IS31Fl3731_H_

#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>

#define ISSI_ADDR_DEFAULT 0x74
#define ISSI_ADDR_VDD 0x77

#define ISSI_REG_CONFIG  0x00
#define ISSI_REG_CONFIG_PICTUREMODE 0x00
#define ISSI_REG_CONFIG_AUTOPLAYMODE 0x08
#define ISSI_REG_CONFIG_AUDIOPLAYMODE 0x18

#define ISSI_CONF_PICTUREMODE 0x00
#define ISSI_CONF_AUTOFRAMEMODE 0x04
#define ISSI_CONF_AUDIOMODE 0x08

#define ISSI_REG_PICTUREFRAME  0x01

#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_AUDIOSYNC 0x06

#define ISSI_COMMANDREGISTER 0xFD
#define ISSI_BANK_FUNCTIONREG 0x0B    // helpfully called 'page nine'


class Adafruit_IS31FL3731 : public Adafruit_GFX {
 public:
  Adafruit_IS31FL3731(uint8_t x=16, uint8_t y=18); 
  boolean begin(uint8_t addr = ISSI_ADDR_DEFAULT, uint8_t addr2 = ISSI_ADDR_VDD);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void clear(void);

  void setLEDPWM(uint16_t lednum, uint8_t pwm, uint8_t bank = 0);
  void audioSync(boolean sync);
  void setFrame(uint8_t b);
  void displayFrame(uint8_t frame);

 protected:
  void selectBank(uint8_t bank);
  void writeRegister8(uint8_t bank, uint8_t reg, uint8_t data, uint8_t addr);
  uint8_t readRegister8(uint8_t bank, uint8_t reg, uint8_t addr);
  uint8_t _i2caddr, _frame ,_i2caddr2;
};

class Adafruit_IS31FL3731_Wing : public Adafruit_IS31FL3731 {
 public:
  Adafruit_IS31FL3731_Wing(void);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
};



#endif
