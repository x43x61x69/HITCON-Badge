///
/// Copyright (C) 2017 Will Huang
///
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included
/// in all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
/// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
/// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
/// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/*
  This file was modified to work with HITCON Badge GPIO.
*/

#ifndef _Control_hpp_
#define _Control_hpp_

#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include "SparkFunLIS3DH.h"

/// This matrix object can be used to control the on-board LED matrix.
extern Adafruit_IS31FL3731 matrix ;

/// Default constructor is I2C, addr 0x18.
extern LIS3DH myIMU;

#define USR_BUTTON (6)      ///< Arduino Pin # for USR button on the side of the board, GPIO37. Active HIGH.
#define USR_LED (3)      ///< Arduino Pin # for USR button on the side of the board, GPIO37. Active HIGH.

#define UP_BUTTON (15)      ///< Arduino Pin # for on-board directional key, GPIO0, active LOW.
#define DOWN_BUTTON (7)     ///< Arduino Pin # for on-board directional key, GPIO36, active LOW.
#define LEFT_BUTTON (16)    ///< Arduino Pin # for on-board directional key, GPIO59, active LOW.
#define RIGHT_BUTTON (17)   ///< Arduino Pin # for on-board directional key, GPIO60, active LOW.
#define CENTER_BUTTON (2)   ///< Arduino Pin # for on-board directional key, GPIO0, active LOW.

//   X
// Y  A
//  B

#define X_BUTTON (10)       ///< Arduino Pin # for on-board button, GPIO32, active LOW.
#define Y_BUTTON (11)       ///< Arduino Pin # for on-board button, GPIO29, active LOW.
#define A_BUTTON (13)       ///< Arduino Pin # for on-board button, GPIO31, active LOW.
#define B_BUTTON (12)       ///< Arduino Pin # for on-board button, GPIO30, active LOW.
#define NO_BUTTON (-1)      ///< No button pressed

#define BUTTONS_SIZE (9)

/// This array contains Arduino Pin # for all active-LOW buttons.
/// Useful for enumerating through all button
extern byte buttons[BUTTONS_SIZE];


/// Initialize button modes (input mode & pull up/down)
void initButtons();

/// Check if any button is pressed
///
/// \returns key code (Arduino Pin #) such as X_BUTTON if a button is pressed.
/// \returns -1 (NO_BUTTON) if no button pressed.
int scanButton();

/// Check if any button is pressed.
/// If a button is pressed, BLOCK until it is released.
/// TODO: current, USR_BUTTON is not supported.
///
/// \returns key code (Arduino Pin #) such as X_BUTTON if a button is pressed and then released.
/// \returns -1 (NO_BUTTON) if no button pressed.
int scanButtonUp();

#endif // _Control_hpp_
