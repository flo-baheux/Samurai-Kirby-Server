#include <iostream>

#include "ClientSocket.h"

ClientSocket::ClientSocket(SOCKET socket, sockaddr_in addr) : _socket{socket}, addr{addr}
{

  // On Windows, client socket IO is inherited from server socket
  // On UNIX, client socket IO is not inherited but rather set as blocking by default
  // To prmessage any confusion we set the socket anyway
#ifdef _WIN32
  u_long mode = 1; // non-zero value for non-blocking mode
  if (ioctlsocket(socket, FIONBIO, &mode) != 0)
  {
    std::cerr << "Winsock error " << WSAGetLastError() << std::endl;
    closesocket(socket);
    WSACleanup();
    throw std::runtime_error("Failed to set socket flag.");
  }
#else
  int flags = fcntl(_socket, F_GETFL, 0);
  if (flags == -1)
  {
    throw std::runtime_error("Failed to retrieve socket flags");
  }

  if (fcntl(_socket, F_SETFL, flags | O_NONBLOCK) == -1)
  {
    throw std::runtime_error("Failed to set socket flags");
  }
#endif
}

ClientSocket::~ClientSocket()
{
#ifdef _WIN32
  closesocket(_socket);
#else
  close(_socket);
#endif
}

char *ClientSocket::getIP() const
{
  return inet_ntoa(addr.sin_addr);
};

int ClientSocket::getPort() const
{
  return ntohs(addr.sin_port);
};

SOCKET ClientSocket::getInternalSocket()
{
  return _socket;
}

void ClientSocket::send(const std::string message)
{
  const std::string messageWithSeparator = message + "\n";
  ::send(_socket, messageWithSeparator.c_str(), messageWithSeparator.length(), 0);
};
