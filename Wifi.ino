#include <ESP8266WiFi.h>

const char *SSID = "36 Ly thuong Kiet";
const char *PASSWORD = "35792468abcd";

void setup()
{
	Serial.begin(115200);
	pinMode(led, OUTPUT);
	startWiFi();
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