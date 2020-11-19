#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "credentials.h"
#include "ESPTemplateProcessor/ESPTemplateProcessor.h"

ESP8266WebServer server(80);

void handleRoot();
String getContentType(String filename);
bool handleFileRead(String path);

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

	server.onNotFound([]() {
		if (!handleFileRead(server.uri()))
			server.send(404, "text/plain", "404: Not found");
	});

	server.begin();
	Serial.println("HTTP server started");
}

void loop()
{
	server.handleClient();
}

String getContentType(String filename)
{
	if (filename.endsWith(".html"))
		return "text/html";
	else
		return "text/plain";
}

bool handleFileRead(String path)
{
	Serial.println("File requested: " + path);
	if (path.endsWith("/"))
		path += "index.html";
	String contentType = getContentType(path);
	if (SPIFFS.exists(path)) {
		File file = SPIFFS.open(path, "r");
		size_t sent = server.streamFile(file, contentType);
		file.close();
		return true;
	}
	Serial.println("File not found");
	return false;
}
