#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char *SSID = "36 Ly thuong Kiet";
const char *PASSWORD = "35792468abcd";
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

	void connectWebSocket()
	{
		webSocket.begin("echo.websocket.org", 80, "/");
		webSocket.onEvent(webSocketEvent);
		webSocket.setReconnectInterval(5000);
	}
	void webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
	{
		switch (type)
		{
		case WStype_DISCONNECTED: // Sự kiện khi client ngắt kết nối
			Serial.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED: // Sự kiện khi client kết nối
			Serial.printf("[WSc] Connected to url: %s\n", payload);
			webSocket.sendTXT("Connected"); // Thông báo kết nối thành công
			break;
		case WStype_TEXT: // Sự kiện khi nhận được thông điệp dạng TEXT
			Serial.printf("[WSc] get text: %s\n", payload);
			webSocket.sendTXT("Hi Server!"); // Gởi thông điệp đến server
			break;
		case WStype_BIN: // Sự kiện khi nhận được thông điệp dạng BINARY
			Serial.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);
			break;
		}
	}
}