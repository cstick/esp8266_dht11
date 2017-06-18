#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#ifndef HEARTBEAT_H
#define HEARTBEAT_H

class Heartbeat
{
    WiFiUDP _udp;
    IPAddress _address;
    unsigned int _port;

  public:
    Heartbeat(IPAddress address, unsigned int port);
    void Send(byte mac[6], long int time, float humidity, float temp);
};

#endif
