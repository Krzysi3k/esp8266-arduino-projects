#include "EspMQTTClient.h"
#include <Arduino.h>
#include <oled.h>


OLED display=OLED(4,5,16,0x3C,128,64,true);
String first, second;

EspMQTTClient client(
    "MY_SSID",
    "MY_PASS",
    "192.168.0.123", // MQTT Broker server ip
    // "MQTTUsername",  // Can be omitted if not needed
    // "MQTTPassword",  // Can be omitted if not needed
    "NodeMCU esp8266",    // Client name that uniquely identify your device
    1883             // The MQTT port, default to 1883. this line can be omitted
);

void displaySpecial()
{
    for (uint_least8_t radius=3; radius<62; radius+=3) {
        delay(20);
        display.draw_circle(64,32,radius);
        if (radius>15) {
            display.draw_circle(64,32,radius-15,OLED::SOLID,OLED::BLACK);
        }
        display.display();
    }
    delay(1000);
    display.clear();
    display.draw_string_P(0,0,PSTR("SDC"),OLED::DOUBLE_SIZE);
    display.display();
    delay(500);
    display.draw_string_P(32,24,PSTR("TECH"),OLED::DOUBLE_SIZE);
    display.display();
    delay(500);
    display.draw_string_P(72,50,PSTR("TALK"),OLED::DOUBLE_SIZE);
    display.display();

    delay(1000);
    for (int i=0; i<6; i++) {
        display.set_invert(true);
        delay(200);
        display.set_invert(false);
        delay(200);
    }

    display.set_scrolling(OLED::HORIZONTAL_RIGHT);
    delay(6000);
    display.set_scrolling(OLED::HORIZONTAL_LEFT);
    delay(6000);
    display.set_scrolling(OLED::DIAGONAL_RIGHT);
    delay(6000);
    display.set_scrolling(OLED::DIAGONAL_LEFT);
    delay(6000);
    display.set_scrolling(OLED::NO_SCROLLING);
    display.display();
    delay(2000);
    display.clear();
}

String searchFor = "lama";
void onConnectionEstablished()
{
    client.subscribe("iot/nodemcu/in", [](const String &topic, const String &payload) {
        String topicAndPayload = "topic: " + topic + " payload: " + payload;
        Serial.println("LOG::received " + topicAndPayload ); 
        client.publish("iot/nodemcu/out", "received message: " + payload);
        display.clear();
        if(searchFor == payload) {
            displaySpecial();
        }
        else {
            display.draw_string_P(0,0,PSTR("received:"), OLED::DOUBLE_SIZE);
            display.display();
            display.draw_string_P(0,32, payload.c_str(), OLED::DOUBLE_SIZE);
            display.display();
        }
    });
    display.draw_string(4,2,"CONNECTED TO MQTT!");
    display.display();
    client.publish("iot/nodemcu/out", "connected!"); // You can activate the retain flag by setting the third parameter to true
}


void setup()
{
    delay(5000);
    Serial.begin(115200);
    display.begin(); 
    // Optional functionalities of EspMQTTClient
    client.enableDebuggingMessages();                                          // Enable debugging messages sent to serial output
    client.enableHTTPWebUpdater();                                             // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
    client.enableOTA();                                                        // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
}


void loop()
{
    client.loop();
}