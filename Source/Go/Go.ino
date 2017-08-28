/*

    HITCON Go (Public Version)

    Copyright (C) 2017 Zhi-Wei Cai.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    ---------------------------------------------------------------------

    * Requirements:
        - LinkIt 7697 Package 0.9.2 or higher is required.
        - Sever MUST sync local time with the Internet and use local timezone!

    * Board Manager URL:
        - http://download.labs.mediatek.com/package_mtk_linkit_7697_index.json

    * D-Pad: URDL -> XABY

    * Hidden Code:
        - Key: 86454e01
        - After boot animation.
        - Show: RIGHT + X + B

    * Show Version:
        - After boot animation.
        - USR + X + B

    * Set Username:
        - After boot animation.
        - Hold: USR

*/

#include  "PrefixHeader.h"

#ifdef DEBUG_MODE
  #define SHOW_UPTIME
  #define SHORT_COOLDOWN
  //#define AUTO_CATCH
#endif

#define IR_PIN 							            4
#define SERIAL_PORT                     115200
#define SERIAL_TIMEOUT                  3000
#define LBLE_SCANNING_WAIT              30 // ANIME_PRESS_A
#define LBLE_USERNAME_WAITING           60000
#ifdef SHORT_COOLDOWN
  #define LBLE_SCANNING_COOLDOWN        15000
  #define HIDDEN_KEY_SCORE              5
#else
  #define LBLE_SCANNING_COOLDOWN        300000
  #define HIDDEN_KEY_SCORE              249 // 0xf9
#endif
#define CHARACTERISTIC_PREFIX           "k"
#define CHARACTERISTIC_SUBFIX           "/k"
#define WRITE_KEY_LENGTH                4
#define USERNAME_LENGTH_MAX            	15
#define FIXED_KEY                       "c239e2e1" // 86454e01
#define TARGET_SERVICE_UUID             "45AE269D-C1A1-4F75-A4C6-E33E8D066CD9"
#define TARGET_CHARACTERISTIC_UUID      "F01BE275-89E6-4D88-ABA2-94483C952887"
#define TARGET_SERVICE_LBLE_UUID        LBLEUuid(TARGET_SERVICE_UUID)
#define TARGET_CHARACTERISTIC_LBLE_UUID LBLEUuid(TARGET_CHARACTERISTIC_UUID)
#define SELF_SERVICE_UUID_PREFIX        0x2CB2
#define SELF_SERVICE_UUID               "2CB2D5C7-5023-4483-82F6-7713D0B77352"
#define SELF_CHARACTERISTIC_UUID        "0909715F-90F6-40AB-820D-C3DD5D997657"

#define BLE_CMD_READ_STATS              "7713D0B77352" // 189ef0f29054

#define NVDM_LENGTH_MAX            	    100
#define NVDM_USER                       "E561408E"
#define NVDM_USER_NAME                  "138D939A"
#define NVDM_USER_CATCH                 "B4CBA982"
#define NVDM_USER_CATCH_NAME            "2CB2D5C7"
#define NVDM_USER_CATCH_CHECKSUM        "0909715F"
#define NVDM_USER_SNAKE_TRY             "3BADACD1"
#define NVDM_USER_SNAKE_SCORE           "05B69B59"

#define USERNAME_TABLE_LENGTH 4
const String userNamePrefixTable[USERNAME_TABLE_LENGTH] = {"Hi, I'm ", "Hello world, ", "", "$whoami: "};
const String userNameSubfixTable[USERNAME_TABLE_LENGTH] = {"!", "!", " in the house!!", " as root"};
#define USERNAME_FILTER_TABLE_LENGTH 2
const String userNameFilterTable[USERNAME_FILTER_TABLE_LENGTH] = {"fuck", "shit"};

#define ANIME_CLEAR                     matrix.clear();
#define ANIME_STRING(...)               drawString(__VA_ARGS__, 1, 15, false, 0, 0);
#define ANIME_STRING_NOT_CONNECTED(...) drawStringWhileNotConnected(__VA_ARGS__, 1, 15);
#define ANIME_STRING_NOT_WRITTEN(...)   drawStringWhileNotWritten(__VA_ARGS__, 1, 15);
#define ANIME_STRING_SLOW(...)          drawString(__VA_ARGS__, 1, 50, false, 0, 0);
#define ANIME_STRING_CANCELABLE(...)    drawString(__VA_ARGS__, 1, 15, true, A_BUTTON, LOW);
#define ANIME_STRING_CANCEL_USR(...)    drawString(__VA_ARGS__, 1, 15, true, USR_BUTTON, HIGH);

#define ANIME_INIT                      animation(icon_hitcon_hat_animation, 23, 0, 0, 18, 16, 10, 1, 0);
#define ANIME_HAT(...)           				animation(icon_hitcon_hat, 8, 0, 0, 18, 16, 200, __VA_ARGS__, 400);
#define ANIME_HAT_CANCEL_USR(...) 			animationCancelable(icon_hitcon_hat, 8, 0, 0, 18, 16, 200, __VA_ARGS__, 400, USR_BUTTON, HIGH);
#define ANIME_SCANNING                  ANIME_HAT(2)
#define ANIME_KEYKACHU_FLASH            flashAnimation(icon_walk, 0, 0, 18, 16, 10, 4);
#define ANIME_KEYKACHU_CATCH            animation(icon_catch, 2, 0, 0, 18, 16, 20, 5, 20);
#define ANIME_HINT_START                ANIME_KEYKACHU_FLASH; ANIME_STRING_CANCELABLE("Wild key appeared!", 1); ANIME_STRING_CANCELABLE("Hold A to catch!", 1);
#define ANIME_PRESS_A                   animationCancelable(icon_walk, 2, 0, 0, 18, 16, 10, 2, 10, A_BUTTON, LOW);
#define ANIME_FLEE                      animation(icon_flee, 18, 0, 0, 18, 16, 5, 1, 0); ANIME_STRING_CANCELABLE("Wild key fled!", 1);
#define ANIME_WAIT                      flashAnimation(icon_wait, 0, 0, 18, 16, 10, 2);
#define ANIME_SHAKE                     animation(icon_wait_shake, 5, 0, 0, 18, 16, 40, 2, 400);
#define ANIME_TAMPERED(...)             flashAnimation(icon_tampered, 0, 0, 18, 16, 10, 2); ANIME_STRING("Someone is tampering with my system..." + String(__VA_ARGS__), 1);
#define ANIME_FAILED                    flashAnimation(icon_failed, 0, 0, 18, 16, 10, 2); ANIME_STRING_CANCELABLE("An error occored!", 1);
#define ANIME_SUCCESS                   animation(icon_key, 39, 0, 0, 18, 16, 5, 1, 5); delay(300); flashAnimation(icon_key + 18 * 16 * 11, 0, 0, 18, 16, 10, 2); delay(100);
#define ANIME_USERNAME                  ANIME_STRING(_userNameString, 1);
#define ANIME_USERNAME_CANCELABLE       ANIME_STRING_CANCEL_USR(_userNameString, 1);
#define DEVICE_NAME_SETUP_INFO          String("Device: ") + _deviceName + String(", Write Prefix: ") + _writeKey, 1
#define ANIME_UPTIME                    ANIME_STRING_CANCELABLE(String("Uptime: ") + millis()/60000 + String(" min"), 1);
#define ANIME_NVDM_RESET_CONFIRM        ANIME_STRING_CANCEL_USR("HOLD USR BUTTON TO FACTORY RESET YOUR BADGE OR PRESS RST TO EXIT. ALL YOUR GAME DATA WILL BE LOST AND WILL BE DISQUALIFIED FROM ALL RELATED REWARDS!!", 10000);
#define ANIME_NVDM_RESET                ANIME_STRING("DATA HAS BEEN RESET.", 1);
#define ANIME_NVDM_WRITE_FAILED         ANIME_STRING("DATA STORE FAILED!", 1);
#define ANIME_NVDM_RESTORE_WARNING      ANIME_STRING_CANCELABLE("Some data failed to be restored. Press RST to reboot your badge or press A to continue.", 10000);


#include <LBLE.h>
#include <LBLEPeriphral.h>
#include <LBLECentral.h>
#include <LFlash.h>
#include "Control.hpp"
#include "icon.c"

unsigned long _catchCount = 0;
unsigned long _snakeCount = 0;
unsigned long _snakeScore = 0;

unsigned long _cooldown = 0;
String _deviceName;
String _userName;
String _userNameString;
String _writeKey;
#define KEY_TABLE_LENGTH 9
const String keyTable[KEY_TABLE_LENGTH] = {"2", "3", "4", "5", "6", "7", "8", "9", "0"};
LBLEService _pServ(SELF_SERVICE_UUID);
LBLECharacteristicString _pChar(SELF_CHARACTERISTIC_UUID, LBLE_WRITE);
LBLEAdvertisementData _advertisement;

void setup()
{
    BUILTIN_LED_ON

    initButtons();

#ifdef DEBUG_MODE
    Serial.begin(SERIAL_PORT);
    Serial.setTimeout(SERIAL_TIMEOUT);
#endif

		LOGD("Version: ");
		LOGDN(VERSION);

    // Setup LBLE

    LOGDN("Init LBLE...");
    LBLE.begin();
    while (!LBLE.ready()) {
        delay(10);
    }
    LOGDN("LBLE ready!");

    _deviceName = LBLE.getDeviceAddress().toString();
    _deviceName.replace(":", "");
    _deviceName = _deviceName.substring(0, 12);

    _pServ.addAttribute(_pChar);
    LBLEPeripheral.addService(_pServ);
    LBLEPeripheral.begin();

    LBLEUuid _serviceUUID(SELF_SERVICE_UUID_PREFIX);
    _advertisement.configAsConnectableDevice(_deviceName.c_str(), SELF_SERVICE_UUID_PREFIX);

    // Initialize LED Matrix
    LOGDN("Init LED Matrix...");
    if (!matrix.begin()) {
      LOGDN("LED Matrix not found.");
      while (1) {
        delay(100);
      }
    }
    matrix.setRotation(1);
    matrix.setTextWrap(false);
    LOGDN("LED Matrix found.");

    // Boot Animation
    LOGDN("Boot Animation...");
    ANIME_CLEAR
    for (int i = 15; i >= 0; i--) {
          drawCode(i, 0, 0);
          drawCode(i, 0, 1);
          drawCode(i, 0, 2);
          drawCode(i, 0, 3);
          drawCode(i, 1, 0);
          drawCode(i, 1, 1);
          drawCode(i, 1, 2);
          drawCode(i, 1, 3);
          delay(50);
    }
    ANIME_INIT
    delay(1000);
    LOGDN("Boot Animation ended.");

    BUILTIN_LED_OFF

		// Username
    _userName = "";
    #ifdef DEBUG_MODE
      _userName = "Guest_" + String(VERSION);
    #endif

		LFlash.begin();
		restoreData();
		setupUserName();

    showVersion();
    _cooldown = millis();
}

void loop()
{
    ANIME_CLEAR

    showC0de();

    LOGD("Cooldown... (");
    LOGD((LBLE_SCANNING_COOLDOWN - (millis() - _cooldown)) / 1000);
    LOGDN(")");

    #ifdef SHOW_UPTIME
      ANIME_UPTIME
    #endif

    const bool shouldScan = millis() - _cooldown > LBLE_SCANNING_COOLDOWN;

    if (digitalRead(USR_BUTTON)) {
      LOGDN("BLE Writing Mode!");
      beginAdvertisement();
      while (digitalRead(USR_BUTTON)) {
          showDeviceName();
          const unsigned long t = millis();
          while (//LBLEPeripheral.connected() ||
                 millis() - t < LBLE_USERNAME_WAITING) {
              if (checkForUsername()) {
                LOGDN("Canceled waiting!");
                break;
              }
              LOGD("Waiting... (");
              LOGD((LBLE_USERNAME_WAITING - (millis() - t)) / 1000);
              LOGDN(")");
          }
      }
      stopAdvertisement();
    } else if (_userName.length() &&
               !shouldScan) {
                  ANIME_USERNAME_CANCELABLE
    }

    if (shouldScan) {
        LOGD("Start scanning...");
        BUILTIN_LED_ON
        LBLECentral.scan();
        if (_userName.length()) {
          ANIME_HAT(1)
          ANIME_USERNAME
        } else {
          ANIME_SCANNING
        }
        LOGD(" (");
        #ifdef DEBUG_MODE
          const unsigned long t = millis() / 1000.f;
          String tStr =  String(t, DEC);
        #endif
        LOGD(tStr);
        LOGDN(")");
        LBLECentral.stopScan();
        BUILTIN_LED_OFF
        for (int i = 0; i < LBLECentral.getPeripheralCount(); ++i) {
            if (checkDeviceList(i)) {
                LOGD("Done, stop for a while...");
                _cooldown = millis();
                LOGDN("");
                break;
            }
        }
    }

    ANIME_HAT(1)
}

bool checkForUsername()
{
  if (LBLEPeripheral.connected()) {
    ANIME_STRING_NOT_WRITTEN(String("Connected, waiting for data... (Write Prefix: ") + _writeKey + String(")"), 1);
  } else {
    ANIME_STRING_NOT_CONNECTED(DEVICE_NAME_SETUP_INFO);
  }
  if (_pChar.isWritten()) {
      const String readStr = _pChar.getValue();
      _pChar.setValue(""); // Clear previous data to prevent prefix leak.
      LOGD("Got: ");
      LOGDN(readStr);
      if (readStr.startsWith(_writeKey)) {
        const int len = readStr.length();
        if (len > WRITE_KEY_LENGTH) {
          const String readCommand = readStr.substring(_writeKey.length());
          if (readCommand == processString(BLE_CMD_READ_STATS)) {
            unsigned long checksum = 0;
            restoreValue(NVDM_USER_CATCH_CHECKSUM, &checksum);
            String checkName = "";
            restoreName(NVDM_USER_CATCH_NAME, &checkName);
            bool chk = (checkName == _deviceName) && (!_catchCount || (_catchCount * 133 + 7 == checksum));
            const String result = chk ? "P" : "F";
            LOGDN(String("Caught: ") + _catchCount + String(" -> ") + result + String(" (") + checksum + String(", ") + checkName + String(")"));
            ANIME_STRING_CANCELABLE(_catchCount + String(":") + result + String(":") + checkName, 10);
          } else {
            _userName = readCommand;
            setupUserName();
            LOGD("New Username: ");
            LOGDN(_userName);
            ANIME_STRING(String("New Username: ") + _userName, 1);
          }
        }
      } else {
        ANIME_STRING_CANCELABLE("Wrong prefix detected, exit...", 1);
      }
      return true;
  } else if (Serial.available()) {
        const String readStr = Serial.readString();
        if (readStr.length()) {
            _userName = readStr;
            setupUserName();
            LOGD("New Username: ");
            LOGDN(_userName);
            ANIME_STRING(String("New Username: ") + _userName, 1);
            return true;
        }
  }
  return false;
}

void setupUserName()
{
    _userName.trim();
    const int len = _userName.length();
    if (len) {
        _userName.replace("\n", "");
		    _userName.replace("\r", "");
        if (len > USERNAME_LENGTH_MAX) {
             _userName = _userName.substring(0, USERNAME_LENGTH_MAX);
        }
        cleanUpUserName();
        if (_userName == "__x43x61x69__") {
            _userNameString = "I am the Designer of Your Badge.";
        } else {
            randomSeed(analogRead(A0));
            const int i = random(0, USERNAME_TABLE_LENGTH);
            _userNameString = userNamePrefixTable[i] + _userName + userNameSubfixTable[i];
        }
    } else {
        _userNameString = "";
    }
}

void cleanUpUserName()
{
    String userNameCopy = _userName;
    userNameCopy.toLowerCase();
    for (int i = 0; i < USERNAME_FILTER_TABLE_LENGTH; i++) {
        const String f = userNameFilterTable[i];
        int pos = -1;
        while ((pos = userNameCopy.indexOf(f, pos + 1)) >= 0) {
            LOGDN("Found " + f + " at index: " + String(pos));
            LOGDN(_userName.substring(0, pos + 1));
            LOGDN( _userName.substring(pos + 3));
            _userName = _userName.substring(0, pos + 1) + "***" + _userName.substring(pos + 4);
        }
    }
    LOGDN("Filtered User Name: " + _userName);
		if (!storeName(NVDM_USER_NAME, _userName)) {
      ANIME_NVDM_WRITE_FAILED
    }
}

void setupWriteKey()
{
  _writeKey = "";
  randomSeed(analogRead(A0));
  for (int i = 0; i < WRITE_KEY_LENGTH; i++) {
    _writeKey += keyTable[random(0, KEY_TABLE_LENGTH)];
  }
  LOGDN("Write Prefix: " + _writeKey);
}

void setupAdvertisement()
{
  if (_deviceName.length()) {
    _pChar.setValue("");
    while (Serial.available()) {
        Serial.read();
    }
    LBLEPeripheral.advertise(_advertisement);
    BUILTIN_LED_ON
    LOGDN(_deviceName + " is now ready.");
  }
}

void beginAdvertisement()
{
  setupWriteKey();
  setupAdvertisement();
}

void stopAdvertisement()
{
  LBLEPeripheral.stopAdvertise();
  LBLEPeripheral.disconnectAll();
  BUILTIN_LED_OFF
}

unsigned long Snake()
{
  // The Snake source was removed for licenseing reasons.
  return 0;
}

// RIGHT + X + B
void showC0de()
{
    if (!digitalRead(USR_BUTTON) &&
        !digitalRead(RIGHT_BUTTON) &&
        !digitalRead(X_BUTTON) &&
         digitalRead(Y_BUTTON) &&
         digitalRead(A_BUTTON) &&
        !digitalRead(B_BUTTON)) {
            delay(500);
            unsigned long score = Snake();
            LOGDN("Score: " + String(score));

            if (score) {
                _snakeCount++;
                storeValue(NVDM_USER_SNAKE_TRY, _snakeCount);
                if (_snakeScore &&
                    score > _snakeScore) {
                      _snakeScore = score;
                      storeValue(NVDM_USER_SNAKE_SCORE, _snakeScore);
                      ANIME_STRING_CANCELABLE("New High Score: " + String(score), 5);
                } else {
                    String str = "Score: " + String(score);
                    if (_snakeScore) {
                        str += " (High Score: " + String(_snakeScore) + ")";
                    }
                    ANIME_STRING_CANCELABLE(str, 5);
                }
                delay(500);
                if (score > HIDDEN_KEY_SCORE) {
                    ANIME_SUCCESS
                    LOGDN("Hidden key!");
                    ANIME_CLEAR
                    const String str = FIXED_KEY;
                    const int length = str.length();
                    String newStr = "Secret key: ";
                    for (int pos = 0; pos < length; pos++) {
                        const String c = str.substring(pos, pos+1);
                        int n = stringToHex(c) * (pos + 1338) % 16;
                        // drawCode(n, pos / 4, pos % 4);
                        newStr += String(n, HEX);
                    }
                    while (digitalRead(A_BUTTON)) {
                        ANIME_STRING_CANCELABLE(newStr, 1);
                    }
                    LOGDN("Exit key...");
                }
            }

            delay(1000);
    }
}

// USR + X + B
void showVersion()
{
    if (digitalRead(USR_BUTTON) &&
        !digitalRead(X_BUTTON) &&
        !digitalRead(B_BUTTON) &&
         digitalRead(A_BUTTON) &&
         digitalRead(Y_BUTTON)) {
            LOGD("Version: ");
            LOGDN(VERSION);
            ANIME_CLEAR
            delay(500);
            bool exit = 0;
            while (!exit) {
                #ifdef DEBUG_MODE
                  ANIME_STRING_CANCELABLE(String("Ver. ") + VERSION + String(" (Debug)"), 1);
                #else
                  ANIME_STRING_CANCELABLE(String("Ver. ") + VERSION, 1);
                #endif
                if (!exit &&
                    !digitalRead(A_BUTTON)) {
                    LOGDN("Exit version...");
                    exit = 1;
                }
                //delay(1);
            }
            ANIME_CLEAR
    }
}

void showDeviceName()
{
  ANIME_STRING(DEVICE_NAME_SETUP_INFO);
}

bool checkCode(const String str)
{
    const int length = str.length();
    int checksum = 0;
    for (int pos = 0; pos < length - 1; pos++) {
        const String c = str.substring(pos, pos+1);
        checksum += stringToHex(c) * (pos + 13);
    }
    const String cv = str.substring(length - 1, length);
    checksum %= 16;
    LOGD("Checksum: ");
    LOGD(stringToHex(cv));
    LOGD(" -> ");
    LOGDN(checksum);
    return checksum == stringToHex(cv);
}

bool checkDeviceList(const int index)
{
  if (!LBLECentral.getServiceUuid(index).isEmpty()) {
    String service = LBLECentral.getServiceUuid(index).toString();
    if (service.equals(TARGET_SERVICE_UUID)) {
      const String name = LBLECentral.getName(index);
      LOGD("Target found: ");
      LOGDN(name);

      ANIME_HINT_START
      #ifndef AUTO_CATCH
      LOGD("Waiting for user action...");
      int count = 0;
      while (count < LBLE_SCANNING_WAIT) {
        if (digitalRead(USR_BUTTON)) {
          LOGDN(" user canceled!");
          return true;
        }
        // Button A Pressed
        if (!digitalRead(A_BUTTON)) {
          break;
        }
        if (!(count % 10)) {
          LOGD(".");
        }
        count++;
        if (count >= LBLE_SCANNING_WAIT) {
          LOGDN(" too late!");
          ANIME_FLEE
          return false;
        }
        // Show animation
        ANIME_PRESS_A
      }
      LOGDN("");
      #else
      ANIME_PRESS_A
      LOGDN("Auto catch mode is on!");
      #endif

      LBLEAddress targetAddr = LBLECentral.getBLEAddress(index);
      const String targetName = LBLECentral.getName(index);
      LOGD("Connecting to " + targetName + "[");
      LOGD(targetAddr);
      LOGD("]...");

      ANIME_KEYKACHU_CATCH
      ANIME_WAIT

      LBLEClient client;

      if (LBLEPeripheral.connected()) {
          LBLEPeripheral.disconnectAll();
      }

      client.connect(targetAddr);

      ANIME_SHAKE

      if (!client.connected()) {
          LOGDN(" failed!");
          ANIME_FAILED
          return false;
      }

      BUILTIN_LED_ON
      LOGDN(" connected!");

      if (client.hasService(TARGET_SERVICE_LBLE_UUID)) {
        String str = client.readCharacteristicString(TARGET_CHARACTERISTIC_LBLE_UUID);

        LOGDN("Disconnecting...");
        client.disconnect();
        BUILTIN_LED_OFF

        int length = str.length();
        const int tagLength = String(CHARACTERISTIC_PREFIX).length() + String(CHARACTERISTIC_SUBFIX).length();
        if (length > tagLength &&
            str.startsWith(CHARACTERISTIC_PREFIX) &&
            str.endsWith(CHARACTERISTIC_SUBFIX)) {
            str = str.substring(1, length - 2);
            LOGD("Key: ");
            LOGDN(str);

            if (!checkCode(str)) {

                String macAddr = targetAddr.toString();
                if (macAddr.length() > 17) {
                    macAddr = macAddr.substring(0, 17);
                }
                macAddr.toLowerCase();

                LOGDN("Code was tempered!");
                ANIME_TAMPERED(" (Name: " + targetName + ", MAC Addr:" + macAddr + ")");
                ANIME_CLEAR
                return false;
            }

            ANIME_SHAKE
            ANIME_WAIT
            ANIME_SUCCESS
            delay(800);

            ANIME_CLEAR

            _catchCount++;
            const unsigned long chk = _catchCount * 133 + 7;
            storeValue(NVDM_USER_CATCH, _catchCount);
            storeValue(NVDM_USER_CATCH_CHECKSUM, chk);
            storeName(NVDM_USER_CATCH_NAME, _deviceName);

            length = str.length();
            String newStr = "Got key: ";
            for (int pos = 0; pos < length; pos++) {
                const String c = str.substring(pos, pos+1);
                unsigned int n = stringToHex(c) * (pos + 1337) % 16;
                //drawCode(n, pos / 4, pos % 4);
                newStr += String(n, HEX);
            }
            newStr += " (valid for 5 mins!)";
            LOGDN(newStr);
            ANIME_STRING_CANCELABLE(newStr, 1);
            #ifndef AUTO_CATCH
            LOGDN("Waiting for user action...");
            int count = 0;
            while (count < 60) {
                if (!digitalRead(A_BUTTON) ||
                    !digitalRead(B_BUTTON) ||
                    !digitalRead(X_BUTTON) ||
                    !digitalRead(Y_BUTTON)) {
                ANIME_CLEAR
                break;
              }
              ANIME_STRING_CANCELABLE(newStr, 1);
              count++;
            }
            #else
            ANIME_CLEAR
            #endif
            LOGDN("Exit!");
            return true;
        } else if (length) {
            LOGD("[ERROR] Error reading characteristic: ");
            LOGDN(str);
            ANIME_FAILED
        } else {
            LOGDN("[ERROR] Characteristic can't be read!");
            ANIME_FAILED
        }

      } else {
        LOGDN("[ERROR] Required services do not exist!");
        LOGDN("Disconnecting...");
        client.disconnect();
        BUILTIN_LED_OFF
      }
    }
  }
  return false;
}

// NVDM

bool storeName(const char *key, const String name)
{
	const char *buf = name.c_str();
	const uint32_t len = name.length();

  LFlashStatus status = LFlash.write(NVDM_USER,
																		 key,
																		 LFLASH_STRING_DATA,
																		 (uint8_t *)buf,
																		 len);
  if (!status) {
    LOGD("[LFlash] " + String(key) + " has been stored successfully! (");
    LOGD(name);
    LOGDN(")");
  } else {
    LOGD("[LFlash] " + String(key) + " stores failed! (");
    LOGD(status);
    LOGDN(")");
  }
  return status == LFLASH_OK;
}

bool restoreName(const char *key, String *name)
{
  char buf[USERNAME_LENGTH_MAX] = {0};
  uint32_t len = USERNAME_LENGTH_MAX;
  LFlashStatus status = LFlash.read(NVDM_USER,
                                    key,
                                    (uint8_t *)buf,
                                    &len);
  if (!status) {
    String userNameRead = String(buf);
    if (userNameRead.length()) {
      *name = userNameRead;
    }
  }
  return status == LFLASH_ITEM_NOT_FOUND || status == LFLASH_OK;
}

bool storeValue(const char *key, const unsigned long value)
{
  void *buf = malloc(sizeof(value));
	const uint32_t len = sizeof(value);
  memcpy(buf, &value, sizeof(value));

  LFlashStatus status = LFlash.write(NVDM_USER,
																		 key,
																		 LFLASH_RAW_DATA,
																		 (uint8_t *)buf,
																		 len);
  if (!status) {
    LOGD("[LFlash] " + String(key) + " has been stored successfully! (");
    LOGD(value);
    LOGDN(")");
  } else {
    LOGD("[LFlash] " + String(key) + " store failed! (");
    LOGD(status);
    LOGDN(")");
  }
  return status == LFLASH_OK;
}

bool restoreValue(const char *key, unsigned long *value)
{
  char buf[NVDM_LENGTH_MAX] = {0};
  uint32_t len = NVDM_LENGTH_MAX;
  LFlashStatus status = LFlash.read(NVDM_USER,
                                    key,
                                    (uint8_t *)buf,
                                    &len);
  if (!status) {
    *value = *((unsigned long *)buf);
  } else {
    LOGD("[LFlash] " + String(key) + " restore failed! (");
    LOGD(status);
    LOGDN(")");
  }
  return status == LFLASH_ITEM_NOT_FOUND || status == LFLASH_OK;
}

void restoreData()
{
	if (!digitalRead(USR_BUTTON) &&
			!digitalRead(X_BUTTON) &&
			!digitalRead(B_BUTTON) &&
			!digitalRead(A_BUTTON) &&
			!digitalRead(Y_BUTTON)) {
				LOGDN("User reset data restoration.");
        ANIME_NVDM_RESET_CONFIRM

        _userName = "";
        storeName(NVDM_USER_NAME, ""); // This didn't work for some reasons...
        storeValue(NVDM_USER_CATCH, 0);
        storeValue(NVDM_USER_CATCH_CHECKSUM, 0);
        storeName(NVDM_USER_CATCH_NAME, "");
        storeValue(NVDM_USER_SNAKE_TRY, 0);
        storeValue(NVDM_USER_SNAKE_SCORE, 0);

				ANIME_NVDM_RESET
	} else {
    bool err = false;
		if (restoreName(NVDM_USER_NAME, &_userName)) {
      LOGD("[LFlash] Restored Username:");
      LOGDN(_userName);
		} else {
  		LOGDN("[LFlash] Failed to restore username!");
      err = true;
		}
		if (restoreValue(NVDM_USER_CATCH, &_catchCount)) {
      LOGD("[LFlash] Restored Catches:");
      LOGDN(_catchCount);
		} else {
  		LOGDN("[LFlash] Failed to restore catches!");
      err = true;
		}
		if (restoreValue(NVDM_USER_SNAKE_TRY, &_snakeCount)) {
      LOGD("[LFlash] Restored Snake Count:");
      LOGDN(_snakeCount);
		} else {
  		LOGDN("[LFlash] Failed to restore Snake count!");
      err = true;
		}
		if (restoreValue(NVDM_USER_SNAKE_SCORE, &_snakeScore)) {
      LOGD("[LFlash] Restored Snake Score:");
      LOGDN(_snakeScore);
		} else {
  		LOGDN("[LFlash] Failed to restore Snake score!");
      err = true;
		}
    if (err) {
      ANIME_NVDM_RESTORE_WARNING
    }
	}
}

// LED Matrix Display
void animation(const unsigned char *arr, int frame_count, int x0, int y0, int x_len, int y_len, int frame_speed, int loops, int loop_delay)
{
    for (int loop = 0; loop < loops; loop++) {
        for (int frame = 0; frame < frame_count; frame++) {
          matrix.displayFrame(frame % 2);
          matrix.setFrame((frame + 1) % 2);
          drawframe(arr + x_len * y_len * frame, x0, y0, x_len, y_len, false);
          delay(frame_speed);
        }
        matrix.displayFrame((frame_count) % 2);
        delay(loop_delay);
    }
}

void animationCancelable(const unsigned char *arr, int frame_count, int x0, int y0, int x_len, int y_len, int frame_speed, int loops, int loop_delay, const int _button, const int _buttonStatus)
{
    for (int loop = 0; loop < loops; loop++) {
        for (int frame = 0; frame < frame_count; frame++) {
          if (digitalRead(_button) == _buttonStatus) {
                ANIME_CLEAR
                return;
          }
          matrix.displayFrame(frame % 2);
          matrix.setFrame((frame + 1) % 2);
          drawframe(arr + x_len * y_len * frame, x0, y0, x_len, y_len, false);
          delay(frame_speed);
        }
        matrix.displayFrame((frame_count) % 2);
        delay(loop_delay);
    }
}

void flashAnimation(const unsigned char *arr, int x0, int y0, int x_len, int y_len, int frame_speed, int loops)
{
    for (int loop = 0; loop < loops * 2; loop++) {
        drawframe(arr, x0, y0, x_len, y_len, loop % 2);
        delay(frame_speed);
    }
    drawframe(arr, x0, y0, x_len, y_len, false);
}

void drawframe(const unsigned char *arr, int x0, int y0, int x_len, int y_len, bool invert)
{
    const float light = 1.0;
    const unsigned factor = invert ? 255 : 0;
    for (int x = 0; x < x_len; x++) {
        for (int y = 0; y < y_len; y++) {
            matrix.drawPixel(x + x0, y + y0, (arr[y * x_len + x] ^ factor) * light);
        }
    }
}

void drawCode(int value, int x, int y)
{
  drawframe(icon_hex + CODE_WIDTH * CODE_HEIGHT * (value + 1),
            CODE_MARGIN_X + y * CODE_WIDTH,
            CODE_MARGIN_Y * (x + 1) + x * CODE_HEIGHT,
            CODE_WIDTH,
            CODE_HEIGHT,
            false);
}

void drawString(const String _str, const int _loops, const int _size, const int _delay, const bool _cancelable, const int _button, const int _buttonStatus)
{
  const int len = -8 * (_str.length());
  for (int loop = 0; loop < _loops; loop++) {
    for (int x = 18; x >= len; x--) {
            ANIME_CLEAR
            if (_cancelable &&
                digitalRead(_button) == _buttonStatus) {
                  return;
            }
            matrix.setCursor(x, 5);
            matrix.setTextSize(_size);
            matrix.print(_str);
            delay(_delay);
    }
  }
}

void drawStringWhileNotConnected(const String _str, const int _loops, const int _size, const int _delay)
{
  const int len = -8 * (_str.length());
  for (int loop = 0; loop < _loops; loop++) {
    for (int x = 18; x >= len; x--) {
            ANIME_CLEAR
            if (LBLEPeripheral.connected() ||
                Serial.available()) {
                  return;
            }
            matrix.setCursor(x, 5);
            matrix.setTextSize(_size);
            matrix.print(_str);
            delay(_delay);
    }
  }
}

void drawStringWhileNotWritten(const String _str, const int _loops, const int _size, const int _delay)
{
  const int len = -8 * (_str.length());
  for (int loop = 0; loop < _loops; loop++) {
    for (int x = 18; x >= len; x--) {
            ANIME_CLEAR
            if (_pChar.isWritten() ||
                Serial.available()) {
                  return;
            }
            matrix.setCursor(x, 5);
            matrix.setTextSize(_size);
            matrix.print(_str);
            delay(_delay);
    }
  }
}

// Misc
int stringToHex(const String str)
{
  return (int)strtol(str.c_str(), 0, 16);
}

String processString(const String str)
{
  String result = "";
  const int length = str.length();
  for (int pos = 0; pos < length; pos++) {
      const String c = str.substring(pos, pos+1);
      int n = stringToHex(c) * (pos + 23) % 16;
      result += String(n, HEX);
  }
  LOGDN(str + String(" -> ") + result);
  return result;
}
