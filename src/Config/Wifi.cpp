#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

class Wifi {
  public:
    static void onConnect(AsyncWebServerRequest * request) {
        int params = request ->params(); 
        String data = request ->getParam(0) ->value().c_str(); 
        Serial.println(data); 
        StaticJsonBuffer <200>jsonBuffer; 
        JsonObject & root = jsonBuffer.parseObject(data); 
        const char * ssid = root["ssid"]; 
        const char * pass = root["pass"]; 
        WiFi.begin(ssid, pass); 
        request ->send(200, "application/json", "{\"status\":\"waitting\"}"); 
    }

    static void onDisconnect(AsyncWebServerRequest * request) {
        WiFi.disconnect(); 
        request ->send(200, "application/json", "{\"status\":true}"); 
    }

    static void onScan(AsyncWebServerRequest * request) {
        int n = WiFi.scanComplete(); 

        if (n == -2) {
            WiFi.scanNetworks(true); 
            request ->send(200, "application/json", "{\"status\":\"progress\"}"); 
        }
        else if (n) {
            Serial.println("scan start"); 
            StaticJsonBuffer <2500>buffScanWifi; 
            JsonArray & mainArray = buffScanWifi.createArray(); 

            JsonObject & root = mainArray.createNestedObject(); 
            Serial.println(1); 
            root["info"] = "scan_wifi"; 
            Serial.println(2); 
            JsonArray & accessPoint = root.createNestedArray("access_points"); 
            Serial.println(3); 
            Serial.print("total net "); 
            Serial.println(n); 
            Serial.println(4); 
            if (n>= 0) {
                Serial.println(5); 
                for (int i = 0; i <n; ++i) {
                    Serial.println("5.6"); 
                    JsonObject & subRoot = accessPoint.createNestedObject(); 
                    subRoot["name"] = WiFi.SSID(i); 
                    subRoot["strength"] = WiFi.RSSI(i); 
                    String encrypType = ""; 
                    switch (WiFi.encryptionType(i)) {
                    case ENC_TYPE_NONE:
                        encrypType = "NONE"; 
                        break; 
                    case ENC_TYPE_WEP:
                        encrypType = "WEP"; 
                        break; 
                    case ENC_TYPE_TKIP:
                        encrypType = "TKIP"; 
                        break; 
                    case ENC_TYPE_CCMP:
                        encrypType = "CCMP"; 
                        break; 
                    case ENC_TYPE_AUTO:
                        encrypType = "AUTO"; 
                        break; 
                    default:
                        break; 
                    }

                    int bars; 
                    int32_t rssi = WiFi.RSSI(i); 
                    //  int bars = map(RSSI,-80,-44,1,6); // this method doesn't refelct the Bars well
                    // simple if then to set the number of bars

                    if (rssi > -55)
                    {
                        bars = 5;
                    }
                    else if (rssi<-55 & rssi> - 65)
                    {
                        bars = 4;
                    }
                    else if (rssi<-65 & rssi> - 70)
                    {
                        bars = 3;
                    }
                    else if (rssi<-70 & rssi> - 78)
                    {
                        bars = 2;
                    }
                    else if (rssi<-78 & rssi> - 82)
                    {
                        bars = 1;
                    }
                    else
                    {
                        bars = 0;
                    }
                    subRoot["quality"] = bars;

                    subRoot["type"] = encrypType;
                }

                WiFi.scanDelete();
                WiFi.scanNetworks(false);
            }
            else
            {
                Serial.printf("status %d\r\n", n);
            }

            Serial.println(6);

            String tmpPrint = "";
            mainArray.printTo(tmpPrint);
            Serial.println(tmpPrint);
            request->send(200, "application/json", tmpPrint);
        }
    }
};