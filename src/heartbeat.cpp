#include "heartbeat.h"

Heartbeat::Heartbeat(IPAddress address, unsigned int port)
{
    _address = address;
    _port = port;

    //_udp.begin(_port);
};

void Heartbeat::Send(byte mac[6], long int time, float humidity, float temp)
{
    // char data[sizeof(byte) * sizeof(mac) + sizeof(long int) + sizeof(float) + sizeof(float)];
    // int bytes = 0;

    // byte *timeBytes = (byte *)&humidity;
    // byte *humidityData = (byte *)&humidity;
    // byte *humidityData = (byte *)&humidity;

    char macString[17];
    sprintf(macString, "%02X%02X%02X%02X%02X%02X",
            mac[0],
            mac[1],
            mac[2],
            mac[3],
            mac[4],
            mac[5]);

    char msg[150];
    sprintf(msg, "%s;%u;%u;%u", macString, time, int(humidity * 100), int(temp * 100));

    // https://github.com/esp8266/Arduino/issues/1252
    _udp.beginPacketMulticast(_address, _port, WiFi.localIP());
    _udp.write(msg);
    //_udp.write(udpBuffer, sizeof(udpBuffer));
    _udp.endPacket();
};