#define VERSION "0.9.27"

#ifndef DEBUG_MODE
  //#define DEBUG_MODE
#endif

#ifdef DEBUG_MODE
  #define LOGD(...)       Serial.print(__VA_ARGS__);
  #define LOGDN(...)      Serial.println(__VA_ARGS__);
#else
  #define LOGD(...)
  #define LOGDN(...)
#endif

#define BUILTIN_LED_ON      digitalWrite(USR_LED, LOW);
#define BUILTIN_LED_OFF     digitalWrite(USR_LED, HIGH);
