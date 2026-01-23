#include <ESP32WiFiProvisioner.h>

ESP32WiFiProvisioner portal;

void setup() {
  Serial.begin(115200);

  portal.addField("device_id", "Device ID");
  portal.addField("location", "Location");
  portal.addField("tank_height", "Tank Height (cm)");

  portal.begin("MyDevice_Setup", "12345678",
    [](const String& ssid,
       const String& password,
       const std::vector<std::pair<String,String>>& extras) {

      Serial.println(ssid);
      Serial.println(password);

      for (auto& kv : extras) {
        Serial.println(kv.first + " = " + kv.second);
      }

      // Save to EEPROM / Preferences
    });
}

void loop() {
  portal.handle();
}
