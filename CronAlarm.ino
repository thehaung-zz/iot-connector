#include "CronAlarms.h"

void setup()
{
	Serial.begin(9600);
	Serial.println("Starting setup...");
	Cron.create("*/2 * * * * *", Repeats, false);
	Serial.println("Ending setup...");
}

void loop()
{
	Cron.delay();
	delay(1000);
}
void Repeats()
{
	Serial.println("2 second timer");
}