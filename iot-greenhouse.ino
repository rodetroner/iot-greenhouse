#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "credentials.h"
#include "ESPTemplateProcessor/ESPTemplateProcessor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

const int one_wire_bus = 4;

unsigned long previous_millis = 0;
const long interval = 10000;

float temperature = 0.0f;

OneWire one_wire(one_wire_bus);
DallasTemperature sensors(&one_wire);
ESP8266WebServer server(80);

void handle_root();

String index_processor(const String& key)
{
	if (key == "TEMPERATURE")
		return String(temperature);

	return "#ERROR#";
}

void handle_root()
{
	if (server.method() == HTTP_GET) {
		Serial.print("Time between open phases (in ms): ");
		Serial.println(server.arg(0));
	}
	if (!ESPTemplateProcessor(server).send(String("/index.html"), index_processor))
		server.send(200, "text/plain", "File not found");
}

void setup()
{
	Serial.begin(115200);
	Serial.println();

	WiFi.begin(ssid, password);

	Serial.print("Connecting");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print('.');
	}
	Serial.println();

	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());

	SPIFFS.begin();

	server.on("/", handle_root);

	server.begin();
	Serial.println("HTTP server started");
}

void loop()
{
	unsigned long current_millis = millis();

	if (current_millis - previous_millis >= interval) {
		previous_millis = current_millis;
		sensors.requestTemperatures();
		temperature = sensors.getTempCByIndex(0);
		Serial.print("Current temperature (in degC):";
		Serial.println(temperature);
	}
	server.handleClient();
}
