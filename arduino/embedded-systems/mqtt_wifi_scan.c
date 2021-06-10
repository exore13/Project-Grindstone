// Author:          Tanase, Dragos 
// Date:            2021/06/10
//
// Embedded Systems
//
// Description:    A quite simple MQTT Client made for an Arduino Nano 33 IoT. 
//                 It will scan the networks around and every 10 seconds it
//                 will send the networks scanned to the broker.
//

#include <ArduinoBLE.h>
#include <ArduinoMqttClient.h>

// File with WiFi credentials
#include "arduino_secrets.h"

// WifiNina include
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ESP8266_ESP12)
  #include <ESP8266WiFi.h>
#endif



// Credentials from arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// Some global variables
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
String scan_dump = String();
int iteration_delay = 10000; // Expressed in ms

// MQTT broker
char broker[] = "192.168.1.70";
int port = 1883;
char topic[] = "se/practica4";



void setup() 
{
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Serial connected!");
}

void loop() 
{
    scan_dump = wifi_scan();
    wifi_on();
    send_mqtt_split(scan_dump);
    wifi_off();
    delay(iteration_delay);
}

// Function used to send MQTT messages in a clean way.
// As connect and desconnect are present, this function
// should not be used on a fast pacin loop
void send_mqtt(String data) 
{
    // Connect to Broker
    mqtt_connect();

    // Keepalive yay
    mqttClient.poll();

    // Printing console info
    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.println(data);

    // Sending MQTT message
    mqttClient.beginMessage(topic);
    mqttClient.println(data);

    // Sending the message
    mqttClient.endMessage();
    delay(2000);

    // Spacing 
    Serial.println(); 
}

// Function used to send MQTT messages in a peculiar way.
// This function will send the string information each 4
// lines. It's made specific for type of data structured on lines. 
// Non serialization standard was considered while making this code.
void send_mqtt_split(String data)
{
    // Connect to Broker
    mqtt_connect();

    // Keepalive yay
    mqttClient.poll();

    // Printing console info
    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.println(data);

    
    int lines = 0;
    String aux = "";
    for(unsigned int i = 0; i < data.length(); i++)
    {
        // Sends 4 lines each time
        if(lines > 3)
        {
            lines = 0;
            mqttClient.beginMessage(topic);
            mqttClient.print(aux);
            mqttClient.endMessage();
            aux = "";
            delay(100);
        }

        if(data[i] == '\n')
        {
            lines++;
            aux += '\n';
            continue;
        }

        aux += data[i];
    }

    // If there are lines ready to be sent, they are sent.
    if(lines != 0)
    {
        mqttClient.beginMessage(topic);
        mqttClient.print(aux);
        mqttClient.endMessage();
        delay(100);
    }


    // Spacing 
    Serial.println(); 
}

// Prepares a formated string listing all detected wireless networks
String wifi_scan() 
{
    String sol = String();
    Serial.println("Scanning WiFi...");
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1) {
        Serial.println("No network found...");
        sol + "No network found...\n";
        return sol;
    }

    // Preparo el mensaje
    sol = "# Wifi Networks: ";
    sol += String(numSsid) + "\n";

    for (int thisNet = 0; thisNet < numSsid; thisNet++) {
        sol += String(thisNet);
        sol += ") ";
        sol += WiFi.SSID(thisNet);
        sol += "\tSignal: ";
        sol += WiFi.RSSI(thisNet);
        sol += " dBm\n";
    }

    return sol;
}

// Disconnects WiFi
void wifi_off() {
    WiFi.disconnect();
    WiFi.end();
    Serial.println("Wifi disconnected");
}

// Connects to the known network
void wifi_on() 
{
    Serial.print("Connecting network: ");
    Serial.println(ssid);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("Connection established!");
}

// Connects to broker
void mqtt_connect()
{
    Serial.println("Connecting Broker...");

    while (!mqttClient.connect(broker, port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());

        Serial.println("Trying again...");
        delay(1000);
    }

    Serial.println("Broker connection established...");
}