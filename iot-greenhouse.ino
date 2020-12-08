#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "credentials.h"
#include "ESPTemplateProcessor/ESPTemplateProcessor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

const int one_wire_bus = 4;
const int valve_pin = 5;

unsigned long previous_temp_millis = 0;
unsigned long previous_valve_millis = 0;

const long temp_measure_interval = 10000;
unsigned long valve_on_interval = 2000;
unsigned long valve_off_interval = 9000;

int valve_state = LOW;

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
		valve_off_interval = server.arg(0).toInt();
		Serial.print("Time between open phases (in ms): ");
		Serial.println(valve_off_interval);
	}
	if (!ESPTemplateProcessor(server).send(String("/index.html"), index_processor))
		server.send(200, "text/plain", "File not found");
}
void setup()
{
	pinMode(valve_pin, OUTPUT);

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

	Serial.println("Valve closed");
}

void loop()
{
	unsigned long current_millis = millis();

	if (current_millis - previous_temp_millis >= temp_measure_interval) {
		previous_temp_millis = current_millis;
		sensors.requestTemperatures();
		temperature = sensors.getTempCByIndex(0);
		Serial.print("Current temperature (in degC): ");
		Serial.println(temperature);
	}

	if (current_millis - previous_valve_millis >= valve_off_interval  &&  valve_state == LOW) {
		previous_valve_millis = current_millis;
		valve_state = HIGH;
		digitalWrite(valve_pin, valve_state);
		Serial.println("Valve open");
		}
	else if (current_millis - previous_valve_millis >= valve_on_interval  &&  valve_state == HIGH) {
		previous_valve_millis = current_millis;
		valve_state = LOW;
		digitalWrite(valve_pin, valve_state);
		Serial.println("Valve closed");
	}

	server.handleClient();
}
