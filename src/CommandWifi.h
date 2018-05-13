#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <stdio.h>
#include "./CommandWifi.cpp"

class CommandWifi {
public:
  String getThisCommandStatus() {
  }

  int doScanWifi() {
  }

  int doConnect(const char * ssid, const char * pass) {
    WiFi.begin(ssid, pass); 
  }

  int doDisconnect() {
  }

  int getAllInfo() {
  }

  int getStatus() {
  }

private:
  int thisCommand = 0; 
}; 
