#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "SSID"
#define STAPSK "password"

#endif

const char *ssid = STASSID;
const char *password = STAPSK;
const int DO = 14;
unsigned long last_event = 0;

void setup()
{
    delay(1000);
    pinMode(DO, INPUT);
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}


void loop()
{
  int output = digitalRead(DO);
  if (output == LOW) {
    if (millis() - last_event > 100) {
      Serial.println("clap sound detected!");
    }
    last_event = millis();
  }
}
