/* Need better work here, like class
  for remote logging and debbuging
*/

#include "UDPLog.h"
#include "WiFiUdp.h"


void udpLogSend(IPAddress _host, uint16_t _port, String _message) {
  WiFiUDP socket ;
  const int udpLocalPort = 5400 ;
  socket.begin(udpLocalPort);
  socket.beginPacket(_host, _port) ;

  //WiFIUDP don´t support String class, so I need to convert this to char[len]
  //before send to another host
  char _buffer[ _message.length() ] ;
  _message.toCharArray(_buffer, _message.length());

  socket.write(_buffer) ;
  socket.endPacket() ;
}
