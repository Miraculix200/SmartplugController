// #############################################################################
// #############################################################################
// #############################################################################

#include "Plug_Controller.h"
#include <WiFi.h>

const char* ssid = "hax";
const char* password = "hax";

IPAddress lampIP = { 192, 168, 0, 28 };
PlugController lamp(lampIP, 9999);

// #############################################################################

void setup()
{
    Serial.begin(115200);
    Serial.println("Booting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    Serial.println("Plug may stop reacting and responding when sending several consecutive commands");

    String response = lamp.setLed(true);
    if (response == "")
        Serial.println("No response from plug or not connected");
    else
        Serial.println(response);
    delay(1000);

    response = lamp.on();
    if (response == "")
        Serial.println("No response from plug or not connected");
    else
        Serial.println(response);
    delay(1000);
       
//    response = lamp.off();

    response = lamp.setLed(false);
    if (response == "")
        Serial.println("No response from plug or not connected");
    else
        Serial.println(response);
    delay(1000);

    response = lamp.getEmeter();
    if (response == "")
        Serial.println("No response from plug or not connected");
    else
        Serial.println(response);
    delay(1000);

    response = lamp.eraseEmeterStats();
    if (response == "")
        Serial.println("No response from plug or not connected");
    else
        Serial.println(response);
    delay(1000);

    response = lamp.getInfo();
    if (response == "")
        Serial.println("No response from plug or not connected");
    else
        Serial.println(response);
    delay(1000);

    response = lamp.countDown(5, false);
    if (response == "")
        Serial.println("No response from plug or not connected");
    else
        Serial.println(response);
    
    Serial.println("Testing done");
}

// #############################################################################

void loop()
{
    delay(1);
}

// #############################################################################
// #############################################################################
// #############################################################################
