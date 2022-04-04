#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

/*======= Define for oled LCD =======*/
/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c // initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1	 //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// constant initialize
const char *SSID = "36 Ly thuong Kiet";
const char *PASSWORD = "35792468abcd";
const int led = 15;
WebSocketsClient webSocket;

long lastRefreshTime = 0;
/*======= Define for temperature sensor MLX90614 =======*/
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

/*======= Define for heartbeat sensor MAX30102 -E12 =======*/
MAX30105 particleSensor;
const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];	  // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; // Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

/*======= Define for led =======*/
#define PIN_LED_RED 15
#define PIN_LED_GREEN 13
#define PIN_LED_BLUE 12

void setup()
{
	Serial.begin(9600);

	/*======= Setup for oled LCD =======*/
	delay(250);
	display.begin(i2c_Address, true);

	// Clear logo of Adafruit
	display.clearDisplay();

	/*======= Set up for temperature sensor MLX90614 =======*/
	if (!mlx.begin())
	{
		Serial.println("Error connecting to MLX sensor. Check wiring.");
		while (1)
	};

	/*======= Set up for temperature sensor MLX90614 =======*/
	if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // Use default I2C port, 400kHz speed
	{
		Serial.println("MAX30105 was not found. Please check wiring/power. ");
		while (1)
	}

	particleSensor.setup();					   // Configure sensor with default settings
	particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
	particleSensor.setPulseAmplitudeGreen(0);  // Turn off Green LED

	/*======= Set up for led =======*/
	pinMode(PIN_LED_RED, OUTPUT);
	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_BLUE, OUTPUT);

	setLedColor(127, 0, 255);
}

void loop()
{
	heartBeatMeasure();
	displayShow(mlx.readObjectTempC(), beatAvg, 0, 2000);
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

void heartBeatMeasure()
{
	long irValue = particleSensor.getIR();

	if (checkForBeat(irValue) == true)
	{
		// We sensed a beat!
		long delta = millis() - lastBeat;
		lastBeat = millis();

		beatsPerMinute = 60 / (delta / 1000.0);

		if (beatsPerMinute < 255 && beatsPerMinute > 20)
		{
			rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the array
			rateSpot %= RATE_SIZE;					  // Wrap variable

			// Take average of readings
			beatAvg = 0;
			for (byte x = 0; x < RATE_SIZE; x++)
				beatAvg += rates[x];
			beatAvg /= RATE_SIZE;
		}
	}

	if (irValue < 50000)
	{
		beatAvg = -1;
	}
}

void displayShow(float temperature, int heartRate, float spo2, int refreshDisplayTime)
{
	if (millis() - lastRefreshTime >= refreshDisplayTime)
	{
		display.clearDisplay();
		delay(20);
		display.setTextSize(1);
		display.setTextColor(SH110X_WHITE);
		display.setCursor(30, 0);
		display.print("MED-H Device");

		// Temperature Title
		display.setCursor(0, 15);
		display.print("Temperature:");
		// Temperature value
		display.setCursor(75, 15);
		display.print(temperature);

		// Heartbeat Title
		display.setCursor(0, 35);
		display.print("Heartbeat:");
		// Heartbeat Value
		if (heartRate == -1)
		{
			display.setCursor(65, 35);
			display.print("No Finger");
		}
		else
		{
			display.setCursor(75, 35);
			display.print(heartRate);
		}

		// SpO2 Title
		display.setCursor(0, 55);
		display.print("SpO2:");
		display.display();
		lastRefreshTime += refreshDisplayTime;
	}
}

void setLedColor(int red, int green, int blue)
{
	analogWrite(PIN_LED_RED, red);
	delay(20);
	analogWrite(PIN_LED_GREEN, green);
	delay(20);
	analogWrite(PIN_LED_BLUE, blue);
	delay(20);
}

void connectWebSocket()
{
	webSocket.begin("echo.websocket.org", 80, "/");
	webSocket.onEvent(webSocketEvent);
	webSocket.setReconnectInterval(5000);
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
	switch (type)
	{
	case WStype_DISCONNECTED:
		Serial.printf("[WSc] Disconnected!\n");
		break;
	case WStype_CONNECTED:
		Serial.printf("[WSc] Connected to url: %s\n", payload);
		webSocket.sendTXT("Connected");
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

void setLedColorByLevel(int level)
{
	switch (level)
	{
	case 1:
		// Green
		setLedColor(0, 128, 0);
		break;
	case 2:
		// Yellow
		setLedColor(255, 255, 0);
		break;
	case 3:
		// Orange
		setLedColor(255, 165, 0);
		break;
	case 4:
		// Red
		setLedColor(255, 0, 0);
		break;
	case 5:
		// Orange/Red
		setLedColor(255, 69, 0);
		break;
	default:
		// White
		setLedColor(127, 0, 255);
		break;
	}
}

