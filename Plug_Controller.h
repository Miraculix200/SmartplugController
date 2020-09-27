// #############################################################################
// #############################################################################
// #############################################################################

// TP-Link HS100/HS110 SmartPlug Controller for ESP8266/ESP32
// by Miraculix200 (not affiliated with TP-Link)
// License: MIT

// Code may stop working any time if TP-Link changes their firmware

// #############################################################################
// #############################################################################
// #############################################################################

#ifndef PLUG_CONTROLLER_H
#define PLUG_CONTROLLER_H

#include <Arduino.h>
#include <WiFiClient.h>

class PlugController {

private:
    IPAddress targetIP;
    uint16_t targetPort;
    static void serializeUint32(char (&buf)[4], uint32_t val);
    static void encrypt(char* data, uint16_t length);
    static void encryptWithHeader(char* out, char* data, uint16_t length);
    static void decrypt(char* input, uint16_t length);
    uint16_t tcpConnect(char* out, const char* cmd, uint16_t length, unsigned long timeout_millis);

public:
    PlugController(IPAddress ip, uint16_t port);
    String sendCmd(String cmd);
    String on();
    String off();
    String getEmeter();
    String getInfo();
    String eraseEmeterStats();
    String setLed(bool power);
    String countDown(uint16_t seconds, bool act);
};

#endif

// #############################################################################
// #############################################################################
// #############################################################################