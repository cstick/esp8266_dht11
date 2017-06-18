#include "heartbeat.h"

Heartbeat::Heartbeat(IPAddress address, unsigned int port){
    _address = address;
    _port = port;

    //_udp.begin(_port);
};

void Heartbeat::Send() {
    _udp.beginPacketMulticast(_address, _port, WiFi.localIP());
    _udp.write("hello");
    //_udp.write(udpBuffer, sizeof(udpBuffer));
    _udp.endPacket();
};