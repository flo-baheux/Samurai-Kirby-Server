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
#include <optional>
#include <memory>

#include "ServerSocket.h"
#include "ConnectedClient.h"
#include "MessageHub.h"
#include "Gameplay.h"

class Server
{
public:
  Server();
  ~Server() {};

  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;

  void start(int port);

private:
  std::unique_ptr<Gameplay> gameplay;

  int nextClientId = 0;

  MessageHub messageHub;
  std::unique_ptr<NetworkMessageBroker> messageBroker;

  std::unique_ptr<ServerSocket> serverSocket{ nullptr };
  std::vector<std::unique_ptr<ConnectedClient>> connectedClients{};

  /**
   * Initialize network API (WinSocks - Only for Windows. It does nothing on UNIX)
   */
  void initNetwork();

  void handleNewClientConnection();

  void handleClientsCommunication();

  std::optional<std::unique_ptr<ClientSocket>> acceptConnection();
};

#endif