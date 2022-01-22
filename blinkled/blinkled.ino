#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "MY_SSID"
#define STAPSK "MY_PASS"

#endif

const char *ssid = STASSID;
const char *password = STAPSK;
const int LED = 4;

void setup()
{
    delay(4000);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED, OUTPUT);
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED, HIGH); // turn the LED on
    delay(500);             // wait for a second
    digitalWrite(LED, LOW);  // turn the LED off
    delay(500);             // wait for a second
    Serial.println("end of loop");
}
