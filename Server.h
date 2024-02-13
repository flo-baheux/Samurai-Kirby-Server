#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cerrno>
#endif

#include "RoomsManager.h"
#include "ConnectedClient.h"

class Server
{
public:
  Server();
  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;
  int serverListen(int port);

private:
  int serverSocket{};
  struct sockaddr_in serverAddr
  {
  };
  RoomsManager *roomsManager;
  std::vector<ConnectedClient *> connectedClients{};

  /**
   * Initialize network API (WinSocks - Only for Windows. It does nothing on UNIX)
   */
  int initNetwork();

  /**
   * Initialize a server socket and properly binds it
   */
  int initServerSocket(int port);

  /**
   * Accept new connection and push new clients to the connectedClients list
   */
  void handleNewClientConnection();

  void removeDisconnectedClients();
};

#endif