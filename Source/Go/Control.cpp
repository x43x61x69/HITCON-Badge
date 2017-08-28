#include "Arduino.h"
#include "Control.hpp"

//#################### LED Matrix ####################
Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

//#################### IMU 3-axis ####################
LIS3DH myIMU(I2C_MODE, 0x18); //Default constructor is I2C, addr 0x18.

//#################### Keypad & buttons ####################
byte buttons[9] = {
    UP_BUTTON,
    DOWN_BUTTON,
    LEFT_BUTTON,
    RIGHT_BUTTON,
    CENTER_BUTTON,
    A_BUTTON,
    B_BUTTON,
    X_BUTTON,
    Y_BUTTON};

void initButtons()
{
  // Keypad and push buttons
  // Active LOW, so we pull-up.
  for (int i = 0; i < BUTTONS_SIZE; i++)
  {
    pinMode(buttons[i], INPUT_PULLUP);
  }

  // the USR button on the side
  pinMode(USR_BUTTON, INPUT);
}

int scanButton()
{
  for (int i = 0; i < BUTTONS_SIZE; i++)
  {
    if (digitalRead(buttons[i]) == 0)
      return buttons[i];
  }

  if(digitalRead(USR_BUTTON))
  {
    return USR_BUTTON;
  }
  
  return NO_BUTTON;
}

int scanButtonUp()
{
  for (int i = 0; i < BUTTONS_SIZE; i++)
  {
    if (digitalRead(buttons[i]) == 0)
    {
      while (digitalRead(buttons[i]) == 0)
      {
        delay(1);
      }
      return buttons[i];
    }
  }
  return NO_BUTTON;
}
