/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.
*/

#include "EspMQTTClient.h"

String searchFor = "lama";
EspMQTTClient client(
    "MY_SSID",
    "MY_PASS",
    "192.168.0.123", // MQTT Broker server ip
    // "MQTTUsername",  // Can be omitted if not needed
    // "MQTTPassword",  // Can be omitted if not needed
    "NodeMCU esp8266",    // Client name that uniquely identify your device
    1883             // The MQTT port, default to 1883. this line can be omitted
);


void onConnectionEstablished()
{
    client.subscribe("iot/nodemcu/in", [](const String &topic, const String &payload) {
        Serial.println("LOG::received topic - " + topic + ", payload - " + payload); 
        client.publish("iot/nodemcu/out", "received message: " + payload);
        if(searchFor == payload) {
            Serial.println("found text");
        }
        else {
            Serial.println("something else");
        }
    });

    client.publish("iot/nodemcu/out", "connected!"); // You can activate the retain flag by setting the third parameter to true
}

// void onMessageReceived(const String& topic, const String& message) {
//     Serial.println(topic + ": " + message);
//     client.publish("iot/nodemcu/out", "received msg, nice!");
// }

void setup()
{
    delay(5000);
    Serial.begin(115200);
    // Optional functionalities of EspMQTTClient
    client.enableDebuggingMessages();                                          // Enable debugging messages sent to serial output
    client.enableHTTPWebUpdater();                                             // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
    client.enableOTA();                                                        // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
    client.enableLastWillMessage("TestClient/lastwill", "I am going offline"); // You can activate the retain flag by setting the third parameter to true
}

void loop()
{
    client.loop();
}