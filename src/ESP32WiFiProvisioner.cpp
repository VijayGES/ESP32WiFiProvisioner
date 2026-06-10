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
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>WiFi Provisioning</title>"
    "<style>"
    "body{"
      "font-family:Arial,sans-serif;"
      "background:#f4f6f8;"
      "margin:0;"
      "padding:20px;"
      "font-size:20px;"
    "}"
    ".container{"
      "max-width:500px;"
      "margin:auto;"
      "background:white;"
      "padding:20px;"
      "border-radius:12px;"
      "box-shadow:0 2px 10px rgba(0,0,0,0.15);"
    "}"
    "h2{"
      "text-align:center;"
      "font-size:32px;"
      "margin-bottom:20px;"
    "}"
    "label{"
      "font-weight:bold;"
      "display:block;"
      "margin-top:15px;"
      "font-size:22px;"
    "}"
    "input{"
      "width:100%;"
      "padding:14px;"
      "font-size:20px;"
      "margin-top:5px;"
      "border:1px solid #ccc;"
      "border-radius:8px;"
      "box-sizing:border-box;"
    "}"
    "button{"
      "width:100%;"
      "padding:15px;"
      "font-size:22px;"
      "margin-top:15px;"
      "border:none;"
      "border-radius:8px;"
      "background:#2196F3;"
      "color:white;"
      "cursor:pointer;"
    "}"
    "button:hover{background:#1976D2;}"
    "#list{"
      "list-style:none;"
      "padding:0;"
      "margin-top:15px;"
    "}"
    "#list li{"
      "padding:15px;"
      "margin-bottom:8px;"
      "background:#eeeeee;"
      "border-radius:8px;"
      "font-size:20px;"
      "cursor:pointer;"
    "}"
    "#list li:hover{background:#dddddd;}"
    "</style>"
    "</head>"
    "<body>"
    "<div class='container'>"
    "<h2>WiFi Setup</h2>"
    "<form method='POST' action='/save'>"

    "<label>SSID</label>"
    "<input name='ssid' placeholder='Select or enter WiFi'>"

    "<label>Password</label>"
    "<input type='password' name='password' placeholder='WiFi Password'>";

    for (auto& f : customFields) {
        html += "<label>" + f.label + "</label>";
        html += "<input name='" + f.name + "'>";
    }

    html +=
    "<button type='submit'>Save Settings</button>"
    "</form>"

    "<button onclick='scan()'>Scan WiFi Networks</button>"

    "<ul id='list'></ul>"

    "<script>"
    "function scan(){"
      "fetch('/scan')"
      ".then(r=>r.json())"
      ".then(j=>{"
        "let ul=document.getElementById('list');"
        "ul.innerHTML='';"
        "j.forEach(s=>{"
          "let li=document.createElement('li');"
          "li.innerHTML='📶 '+s;"
          "li.onclick=function(){"
             "document.querySelector('[name=ssid]').value=s;"
          "};"
          "ul.appendChild(li);"
        "});"
      "});"
    "}"
    "</script>"

    "</div>"
    "</body>"
    "</html>";

    return html;
}

