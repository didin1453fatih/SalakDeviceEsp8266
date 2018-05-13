#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <AsyncMqttClient.h>
// #include <ArduinoOTA.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <DNSServer.h>
#include <stdio.h>
#include "./config/Config.cpp"

#define MQTT_HOST IPAddress(192, 168, 0, 104)
#define MQTT_PORT 3000

AsyncWebServer server(80);
DNSServer dnsServer;

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
Ticker publishTicker;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

void connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  // mqttClient.setClientId(ESP.getChipId());
  mqttClient.setCredentials("budi", "pass");
  mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
}

void publishTick()
{
  uint16_t packetIdPub1 = mqttClient.publish("lampu/kamar", 1, true, "test 2");
  Serial.print("Publishing at QoS 1, packetId: ");
  Serial.println(packetIdPub1);
}

void onMqttConnect(bool sessionPresent)
{
  uint16_t packetIdSub = mqttClient.subscribe("esp", 1);
  Serial.print("Subscribing at QoS 1, packetId: ");
  Serial.println(packetIdSub);
  // publishTicker.attach(2,publishTick);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected())
  {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  // Serial.print("Topic: ");
  // Serial.println(topic);
  String payS = String(payload);
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  Serial.print("  payload: ");
  Serial.println(payS);
  if (payS == "on")
  {
    digitalWrite(14, HIGH);
  }
  else if (payS == "off")
  {
    digitalWrite(14, LOW);
  }
}

void onMqttPublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

// Track changes of the connection state
void wiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {
  case WIFI_EVENT_STAMODE_CONNECTED:
    Serial.println("WIFI_EVENT_STAMODE_CONNECTED");
    break;
  case WIFI_EVENT_STAMODE_DISCONNECTED:
    Serial.println("WIFI_EVENT_STAMODE_DISCONNECTED");
    break;
  case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
    Serial.println("WIFI_EVENT_STAMODE_AUTHMODE_CHANGE");
    break;
  case WIFI_EVENT_STAMODE_GOT_IP:
    Serial.println("WIFI_EVENT_STAMODE_GOT_IP");
    break;
  case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
    Serial.println("WIFI_EVENT_STAMODE_DHCP_TIMEOUT");
    break;
  case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
    Serial.println("WIFI_EVENT_SOFTAPMODE_STACONNECTED");
    break;
  case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
    Serial.println("WIFI_EVENT_SOFTAPMODE_STADISCONNECTED");
    break;
  case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
    Serial.println("WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED");
    break;
  case WIFI_EVENT_MAX:
    Serial.println("WIFI_EVENT_MAX");
    break;
  case WIFI_EVENT_MODE_CHANGE:
    Serial.println("WIFI_EVENT_MODE_CHANGE");
    break;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.setDebugOutput(false);

  IPAddress apIP(192, 168, 1, 1);
  WiFi.hostname("salak");
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("salak");

  dnsServer.setTTL(300);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(53, "www.salak.local", apIP);

  WiFi.onEvent(wiFiEvent);

  SPIFFS.begin();
  Config config;

  config.start(server);

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  pinMode(14, OUTPUT);
}

void loop()
{
  dnsServer.processNextRequest();
  // LED: LOW = on, HIGH = off
  // Serial.println("Start blinking");
  // // for (int i = 0; i < 20; i++)
  // // {
  //   digitalWrite(14, LOW);
  //   delay(1000);
  //   digitalWrite(14, HIGH);
  //   delay(1000);
  // }
}
