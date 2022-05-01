#include <ArduinoJson.h>
#include "CronAlarms.h"
#include <stdlib.h>

// constant initialize
const char *DEFAULT_WS_EVENT_HANDLER = "sync";

// JsonBuilder Initialize
StaticJsonDocument<200> doc;

void setup()
{
  Serial.begin(9600);

  // Cronjob
  Cron.create("*/10 * * * * *", sendTemperatureToServer, false);
}

void loop()
{
  Cron.delay();
}


void sendTemperatureToServer()
{
  // with arduinoJson
  doc["macAddress"] = WiFi.macAddress();
  doc["event"] = DEFAULT_WS_EVENT_HANDLER;
  // data declare with array index
  // 0 is temperature
  // 1 is heartbeat
  // 2 is spO2
  JsonArray data = doc.createNestedArray("data");
  data.add(mlx.readObjectTempC());
  serializeJson(doc, Serial);
}