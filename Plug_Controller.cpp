// #############################################################################
// #############################################################################
// #############################################################################

// TP-Link HS100/HS110 SmartPlug Controller for ESP8266/ESP32
// by Miraculix200 (not affiliated with TP-Link)
// License: MIT

// Code may stop working any time if TP-Link changes their firmware

// List of other commands: 
// https://github.com/softScheck/tplink-smartplug/blob/master/tplink-smarthome-commands.txt

// #############################################################################
// #############################################################################
// #############################################################################

#include "Plug_Controller.h"

// #############################################################################

PlugController::PlugController(IPAddress ip, uint16_t port)
{
    targetIP = ip;
    targetPort = port;
}

// #############################################################################

void PlugController::serializeUint32(char (&buf)[4], uint32_t val)
{
    buf[0] = (val >> 24) & 0xff;
    buf[1] = (val >> 16) & 0xff;
    buf[2] = (val >> 8) & 0xff;
    buf[3] = val & 0xff;
}

// #############################################################################

void PlugController::decrypt(char* input, uint16_t length)
{
    uint8_t key = 171;
    uint8_t next_key;
    for (uint16_t i = 0; i < length; i++) {
        next_key = input[i];
        input[i] = key ^ input[i];
        key = next_key;
    }
}

// #############################################################################

void PlugController::encrypt(char* data, uint16_t length)
{
    uint8_t key = 171;
    for (uint16_t i = 0; i < length + 1; i++) {
        data[i] = key ^ data[i];
        key = data[i];
    }
}

// #############################################################################

void PlugController::encryptWithHeader(char* out, char* data, uint16_t length)
{
    char serialized[4];
    serializeUint32(serialized, length);
    encrypt(data, length);
    memcpy(out, &serialized, 4);
    memcpy(out + 4, data, length);
}

// #############################################################################

String PlugController::getInfo()
{
    const String cmd = "{\"system\":{\"get_sysinfo\":{}}}";
    return sendCmd(cmd);
}

// #############################################################################

String PlugController::on()
{
    const String cmd = "{\"system\":{\"set_relay_state\":{\"state\":1}}}";
    return sendCmd(cmd);
}

// #############################################################################

String PlugController::off()
{
    const String cmd = "{\"system\":{\"set_relay_state\":{\"state\":0}}}";
    return sendCmd(cmd);
}

// #############################################################################

String PlugController::eraseEmeterStats()
{
    const String cmd = "{\"emeter\":{\"erase_emeter_stat\":null}}";
    return sendCmd(cmd);
}

// #############################################################################

String PlugController::countDown(uint16_t seconds, bool act)
{
    String cmd = "{\"count_down\":{\"add_rule\":{\"enable\":1,\"delay\":";
    cmd += String(seconds);
    cmd += ",\"act\":";
    if (act)
        cmd += "1,\"name\":\"turn on\"}}}";
    else
        cmd += "0,\"name\":\"turn off\"}}}";

    const String delete_all_rules = "{\"count_down\":{\"delete_all_rules\":null}}";
    sendCmd(delete_all_rules);

    return sendCmd(cmd);
}

// #############################################################################

String PlugController::getEmeter()
{
    const String cmd = "{\"emeter\":{\"get_realtime\":{}}}";
    return sendCmd(cmd);
}

// #############################################################################

String PlugController::setLed(bool power)
{
    String cmd = "{\"system\":{\"set_led_off\":{\"off\":1}}}";
    if (power) {
        cmd = "{\"system\":{\"set_led_off\":{\"off\":0}}}";
    } 
    return sendCmd(cmd);
}

// #############################################################################

String PlugController::sendCmd(String cmd)
{
    char encrypted[cmd.length() + 4];
    encryptWithHeader(encrypted, const_cast<char*>(cmd.c_str()), cmd.length());
    char response[2048] = { 0 };
    uint16_t length = this->tcpConnect(response, encrypted, cmd.length() + 4, 5000);
    if (length > 0)
        decrypt(response, length - 4);
    else
        return String("");
    return String(response);
}

// #############################################################################

uint16_t PlugController::tcpConnect(char* out, const char* cmd, uint16_t length, unsigned long timeout_millis)
{
    WiFiClient plug_client;
    if (plug_client.connect(this->targetIP, this->targetPort)) 
    {
        delay(10);
        plug_client.write(cmd, length);

        // give the plug some time to think about its response ^^ 
        // otherwise there may be no response
        // while waiting, pat the dog for ESP8266
        unsigned long start = millis();
        while(millis() - start < 500)
        {
            delay(10); // this includes yield() to pat the dog
        }
        
        start = millis();
        char buf[2048] = { 0 };
        while (plug_client.connected()) {
            if (plug_client.available()) {
                delay(10);
                int len = plug_client.read((uint8_t*)buf, 2048);

                // necessary for decryption
                // buf + 4 strips 4 byte header
                // valread - 3 leaves 1 byte for terminating null character
                strncpy(out, buf + 4, len - 3);

                delay(10);
                plug_client.flush();
                return len;
            }

            if (millis() - start >= timeout_millis) {
                // connection timeout
                break;
            }
            delay(0);
        }
        // timeout/not connected
        delay(10);
        plug_client.flush();
    }
    return 0;
}

// #############################################################################
// #############################################################################
// #############################################################################

// Taxation == theft
// Contracts without >=2 matching declarations of intent are void
// Freedom of speech > your ego/feelings

// #############################################################################
// #############################################################################
// #############################################################################