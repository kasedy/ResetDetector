#include <ResetDetector.h>

#include <ESP8266WiFi.h>

void setupWifi(bool resetPassword) 
{
  if (resetPassword) {
    WiFi.disconnect();
  } else {
    WiFi.setAutoConnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin();
  }

  if (resetPassword || WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.beginSmartConfig();
    while(true) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        if (resetPassword && !WiFi.smartConfigDone()) {
          continue;
        }
        WiFi.stopSmartConfig();
        delay(500);
        break;
      }
    }
  }
}

void setup() { 
  int resetCount = ResetDetector::execute(2000);
  pinMode(LED_BUILTIN, OUTPUT);
  setupWifi(resetCount >= 3);
}

void loop() {}