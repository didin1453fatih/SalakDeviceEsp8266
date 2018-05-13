#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <stdio.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "./Wifi.cpp"
#include "./Utill.cpp"
class Config {
  public:
    Wifi wifi; 
    Utill utill;
    void start(AsyncWebServer & server) {
        server.on("/ping", HTTP_GET,  &utill.onPing ); 
        server.on("/wifiScan", HTTP_GET,  & wifi.onScan); 
        server.on("/wifiConnect", HTTP_POST,  & wifi.onConnect); 
        server.on("/wifiDisconnect", HTTP_GET,  & wifi.onDisconnect); 

        server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm"); 

        server.onNotFound([](AsyncWebServerRequest * request) {
            request ->send(404); 
        }); 
        server.begin(); 
    }
}; 
