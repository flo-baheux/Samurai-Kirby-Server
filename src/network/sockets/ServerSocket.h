#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#endif

class ServerSocket {
public:
  ServerSocket(int port);
  ~ServerSocket();

  ServerSocket() = delete;
  ServerSocket(const ServerSocket &) = delete;

  int getInternalSocket();

private:
  struct sockaddr_in serverAddr {};
  int _socket;
};