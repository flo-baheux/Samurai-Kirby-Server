#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
typedef int SOCKET
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#endif

#include <string>
#include "ServerSocket.h"

class ClientSocket {
public:
  ClientSocket(SOCKET, sockaddr_in);
  ~ClientSocket();

  ClientSocket() = delete;
  ClientSocket(const ClientSocket &) = delete;

  char *getIP() const;
  int getPort() const;
  void send(const std::string message);
  SOCKET getInternalSocket();

private:
  SOCKET _socket;
  struct sockaddr_in addr {};
};

#endif