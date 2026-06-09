#ifndef ESP32_WIFI_PROVISIONER_H
#define ESP32_WIFI_PROVISIONER_H

#if defined(ESP32)

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

#else
#error "Unsupported board. Only ESP32 and ESP8266 are supported."
#endif

#include <vector>
#include <functional>

class ESP32WiFiProvisioner {
public:
    struct Field {
        String name;
        String label;
    };

    using ResultCallback =
        std::function<void(const String& ssid,
                           const String& password,
                           const std::vector<std::pair<String, String>>& extras)>;

    ESP32WiFiProvisioner();

    void begin(const char* apSsid,
               const char* apPassword,
               ResultCallback cb);

    void addField(const String& name, const String& label);
    void handle();

private:
    void startAP();
    void setupServer();
    void handleRoot();
    void handleSave();
    void handleScan();

    String generateHTML();

    DNSServer dns;

	#if defined(ESP32)
		WebServer server;
	#elif defined(ESP8266)
		ESP8266WebServer server;
	#endif

	bool running;

    ResultCallback callback;
    std::vector<Field> customFields;
};

#endif
