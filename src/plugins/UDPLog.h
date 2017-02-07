#ifndef UDPLog_H
#define UDPLog_H
#include "WiFiUdp.h"


extern WiFiUDP socket;

void udpLogSend(IPAddress _host, uint16_t _port, String _message) ;

#endif
