#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "credentials.h"
#include "ESPTemplateProcessor/ESPTemplateProcessor.h"

ESP8266WebServer server(80);

void handleRoot();
String getContentType(String filename);
bool handleFileRead(String path);

String indexProcessor(const String& key)
{
	if (key == "TEMPERATURE")
		return "?";

	return "#ERROR#";
}

void handleRoot()
{
	if (!ESPTemplateProcessor(server).send(String("/index.html"), indexProcessor))
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

	server.on("/", handleRoot);

	server.begin();
	Serial.println("HTTP server started");
}

void loop()
{
	server.handleClient();
}
