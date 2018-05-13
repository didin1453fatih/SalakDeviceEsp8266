#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

class Utill {
    public:
    static void onPing(AsyncWebServerRequest * request) {
        String wifiStatus; 

        switch (WiFi.status()) {
            case WL_IDLE_STATUS:
                wifiStatus = "wifi idle"; 
                break; 
            case WL_NO_SSID_AVAIL:
                wifiStatus = "wifi not avaible"; 
                break; 
            case WL_CONNECTED:
                wifiStatus = "wifi connected"; 
                break; 
            case WL_CONNECT_FAILED:
                wifiStatus = "wifi not connected"; 
                break; 
            case WL_CONNECTION_LOST:
                wifiStatus = "wifi lost"; 
                break; 
            case WL_DISCONNECTED:
                wifiStatus = "wifi disconnected"; 
                break; 
            default:
                break; 
        }

        StaticJsonBuffer <2000>jsonBuffer; 
        JsonObject & root = jsonBuffer.createObject(); 
        JsonObject & info = root.createNestedObject("info"); 
        JsonObject & wifi = info.createNestedObject("wifi"); 
        wifi["ssid"] = WiFi.SSID(); 
        wifi["status"] = wifiStatus; 
        wifi["signal"] = WiFi.RSSI(); 
        wifi["ip"] = WiFi.localIP().toString(); 


        int bars; 
        int32_t rssi = WiFi.RSSI(); 
        
        if (rssi>-55) {
            bars = 5; 
        }else if (rssi <-55 & rssi>-65) {
            bars = 4; 
        }else if (rssi <-65 & rssi>-70) {
            bars = 3; 
        }else if (rssi <-70 & rssi>-78) {
            bars = 2; 
        }else if (rssi <-78 & rssi>-82) {
            bars = 1; 
        }else {
            bars = 0; 
        }
        wifi["quality"] = bars; 
        String tmpPrint = ""; 

        JsonObject & broker = info.createNestedObject("broker"); 
        broker["server"] = "name server"; 
        broker["port"] = 10; 
        broker["ping"] = 1000; 

        root.printTo(tmpPrint); 
        Serial.println(tmpPrint); 
        request ->send(200, "application/json", tmpPrint); 
    }

}; 