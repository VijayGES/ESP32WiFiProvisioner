#include "ESP32WiFiProvisioner.h"

ESP32WiFiProvisioner::ESP32WiFiProvisioner()
: server(80), running(false) {}

void ESP32WiFiProvisioner::begin(const char* apSsid,
                                 const char* apPassword,
                                 ResultCallback cb) {
    callback = cb;
    startAP();
    setupServer();
    running = true;
}

void ESP32WiFiProvisioner::addField(const String& name, const String& label) {
    customFields.push_back({name, label});
}

void ESP32WiFiProvisioner::startAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32_Setup", "12345678");
    dns.start(53, "*", WiFi.softAPIP());
}

void ESP32WiFiProvisioner::setupServer() {
    server.on("/", HTTP_GET, [&]() { handleRoot(); });
    server.on("/save", HTTP_POST, [&]() { handleSave(); });
    server.on("/scan", HTTP_GET, [&]() { handleScan(); });
    server.onNotFound([&]() { handleRoot(); });
    server.begin();
}

void ESP32WiFiProvisioner::handle() {
    if (!running) return;
    dns.processNextRequest();
    server.handleClient();
}

void ESP32WiFiProvisioner::handleRoot() {
    server.send(200, "text/html", generateHTML());
}

void ESP32WiFiProvisioner::handleScan() {
    int n = WiFi.scanNetworks();
    String json = "[";
    for (int i = 0; i < n; i++) {
        json += "\"" + WiFi.SSID(i) + "\"";
        if (i < n - 1) json += ",";
    }
    json += "]";
    server.send(200, "application/json", json);
}

void ESP32WiFiProvisioner::handleSave() {
    if (!callback) return;

    String ssid = server.arg("ssid");
    String password = server.arg("password");

    std::vector<std::pair<String, String>> extras;
    for (auto& f : customFields) {
        extras.push_back({f.name, server.arg(f.name)});
    }

    callback(ssid, password, extras);

    server.send(200, "text/html",
        "<h2>Saved successfully.<br>You may reboot the device.</h2>");
}

String ESP32WiFiProvisioner::generateHTML() {
    String html =
        "<!DOCTYPE html><html><body>"
        "<h2>WiFi Setup</h2>"
        "<form method='POST' action='/save'>"
        "SSID:<br><input name='ssid'><br>"
        "Password:<br><input type='password' name='password'><br>";

    for (auto& f : customFields) {
        html += f.label + ":<br><input name='" + f.name + "'><br>";
    }

    html +=
        "<br><button type='submit'>Save</button></form>"
        "<button onclick='scan()'>Scan WiFi</button>"
        "<ul id='list'></ul>"
        "<script>"
        "function scan(){fetch('/scan').then(r=>r.json()).then(j=>{"
        "let ul=document.getElementById('list');ul.innerHTML='';"
        "j.forEach(s=>{let li=document.createElement('li');"
        "li.innerHTML=s;li.onclick=()=>document.querySelector('[name=ssid]').value=s;"
        "ul.appendChild(li);});});}"
        "</script></body></html>";

    return html;
}
