#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <string.h>

const char *SSID = "36 Ly thuong Kiet";
const char *PASSWORD = "35792468abcd";

const char *event = "{\"event\":\"sync\",\"data\": {\"canal\":";
const char *temp = "10}}";

const int led = 15;
WebSocketsClient webSocket;
void setup()
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  startWiFi();
  connectWebSocket();
}
void loop()
{
  webSocket.loop();
}
void startWiFi()
{
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n");
  Serial.println("Connection established!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectWebSocket()
{
  webSocket.begin("192.168.1.19", 2022, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  char *data;
  data = "\0";
  strcpy(data, event);
  strcat(data, temp);
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
    Serial.printf("[WSc] Connected to url: %s\n", payload);
    webSocket.sendTXT(data);
    Serial.printf("[WSc] Connected to url: %s\n", data);
    break;
  case WStype_TEXT:
    Serial.printf("[WSc] get text: %s\n", payload);
    webSocket.sendTXT("Hi Server!");
    break;
  case WStype_BIN:
    Serial.printf("[WSc] get binary length: %u\n", length);
    hexdump(payload, length);
    break;
  }
}