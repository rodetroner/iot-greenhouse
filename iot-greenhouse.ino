#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "credentials.h"

ESP8266WebServer server(80);

void handleRoot();

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

	server.on("/", handleRoot);

	server.begin();
	Serial.println("HTTP server started");
}

void loop()
{
	server.handleClient();
}

void handleRoot()
{
	server.send(200, "text/plain", "Hello world!");
}
