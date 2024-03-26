#include <iostream>

#include "ServerSocket.h"

ServerSocket::ServerSocket(int port) {
  struct sockaddr_in serverAddr;
  if ((_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    int err = WSAGetLastError();
#ifdef _WIN32
    std::cerr << "Winsock error " << err << std::endl;
    WSACleanup();
#else
    perror("Error creating socket");
#endif
    throw std::runtime_error("Failed to open server socket");
  }

#ifdef _WIN32
  u_long mode = 1; // non-zero value for non-blocking mode
  if (ioctlsocket(_socket, FIONBIO, &mode) != 0) {
    int err = WSAGetLastError();
    std::cerr << "Winsock error " << err << std::endl;
    closesocket(_socket);
    WSACleanup();
    throw std::runtime_error("Failed to set socket to non-blocking mode");
  }
#else
  int flags = fcntl(_socket, F_GETFL, 0);
  if (flags == -1) {
    throw std::runtime_error("Failed to retrieve socket flags");
  }
  if (fcntl(_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
    throw std::runtime_error("Failed to set socket to non-blocking mode");
  }
#endif

  // Set up server address struct
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(port);

  // Bind the socket
  if (bind(_socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
    int err = WSAGetLastError();

#ifdef _WIN32
    std::cerr << "Winsock error " << err << std::endl;
    closesocket(_socket);
    WSACleanup();
#else
    perror("Error binding socket");
    close(serverSocket);
#endif
    throw std::runtime_error("Failed to bind socket");
  }

  // Listen for incoming connections
  if (listen(_socket, 10) == -1) {
    perror("Error listening for connections");
#ifdef _WIN32
    std::cerr << "Winsock error " << WSAGetLastError() << std::endl;
    closesocket(_socket);
    WSACleanup();
#else
    close(_socket);
#endif
    throw std::runtime_error("Failed to listen for connections");
  }
}

ServerSocket::~ServerSocket() {
#ifdef _WIN32
  closesocket(_socket);
  WSACleanup();
#else
  close(_socket);
#endif
}

SOCKET ServerSocket::getInternalSocket() {
  return _socket;
}
