#include <iostream>
#include <algorithm>

#include "Server.h"

Server::Server()
{
  initNetwork();
};

void Server::initNetwork()
{
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    throw new std::runtime_error("Failed to initialize Winsock");
  }
#endif
};

void Server::start(int port)
{
  serverSocket = std::make_unique<ServerSocket>(port, NON_BLOCKING);
  std::cout << "Server listening on port " << port << std::endl;

  while (true)
  {
    try
    {
      handleNewClientConnection();
      handleClientsCommunication();
    }
    catch (std::exception e)
    {
      std::cerr << e.what() << std::endl;
    }
  }
};

void Server::handleNewClientConnection()
{
  // If server is blocking, acceptConnection will block
  std::optional<std::unique_ptr<ClientSocket>> clientSocket = acceptConnection();
  if (!clientSocket.has_value())
  {
    return;
  }

  ConnectedClient *client = new ConnectedClient{std::move(clientSocket.value()), std::make_unique<NetworkEventHub>(eventHub), nextClientId++};
  connectedClients.push_back(client);

  std::cout << "Connection accepted from " << client->getIP() << ":" << client->getPort() << std::endl;
};

void Server::handleClientsCommunication()
{
  for (auto it = connectedClients.begin(); it != connectedClients.end();)
  {
    ConnectedClient *client = *it;
    client->receiveFromNetwork();
    client->sendToNetwork();
    it = client->isDisconnected() ? connectedClients.erase(it) : std::next(it);
  }
}

std::optional<std::unique_ptr<ClientSocket>> Server::acceptConnection()
{
  SOCKET socket;
  struct sockaddr_in clientAddr
  {
  };
  socklen_t clientAddrLen{sizeof(clientAddr)};

  if ((socket = accept(serverSocket->getInternalSocket(), (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1)
  {
    SOCKET_IO serverSocketIOType = serverSocket->getSocketIOType();
#ifdef _WIN32
    int error = WSAGetLastError();
    if ((serverSocketIOType == BLOCKING) || (serverSocketIOType == NON_BLOCKING && error != WSAEWOULDBLOCK))
    {
      throw std::runtime_error("Failed to accept connection. WSA Error " + error);
    }
    else
    {
      return std::nullopt;
    }
#else
    if ((serverSocketIOType == BLOCKING) || (serverSocketIOType == NON_BLOCKING && !(errno == EWOULDBLOCK || errno == EAGAIN)))
    {
      throw std::runtime_error("Failed to accept connection");
    }
    else
    {
      return std::nullopt;
    }
#endif
  }
  return std::make_unique<ClientSocket>(socket, clientAddr, NON_BLOCKING);
}