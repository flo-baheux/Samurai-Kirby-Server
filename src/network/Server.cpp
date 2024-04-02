#include <iostream>
#include <algorithm>

#include "Server.h"
#include <thread>

Server::Server() {
  initNetwork();
  gameplay = std::make_unique<Gameplay>(std::make_unique<GameplayMessageBroker>(messageHub));
  messageBroker = std::make_unique<NetworkMessageBroker>(messageHub);
};

void Server::initNetwork() {
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    throw std::runtime_error("Failed to initialize Winsock");
  }
#endif
};

void Server::start(int port) {
  serverSocket = std::make_unique<ServerSocket>(port);
  std::cout << "Server listening on port " << port << std::endl;

  std::thread t([this]() { gameplay->start(); });

  while (true) {
    try {
      handleNewClientConnection();
      handleClientsCommunication();
    } catch (std::runtime_error e) {
      std::cerr << e.what() << std::endl;
    }
  }
};

void Server::handleNewClientConnection() {
  // If server is blocking, acceptConnection will block
  std::optional<std::unique_ptr<ClientSocket>> clientSocket = acceptConnection();
  if (!clientSocket.has_value()) {
    return;
  }

  std::unique_ptr<ConnectedClient> client = std::make_unique<ConnectedClient>(
    std::move(clientSocket.value()),
    *messageBroker,
    nextClientId++);

  std::cout << "Connection accepted from " << client->getIP() << ":" << client->getPort() << std::endl;

  connectedClients.push_back(std::move(client));
};

void Server::handleClientsCommunication() {
  for (auto it = connectedClients.begin(); it != connectedClients.end();) {
    std::unique_ptr<ConnectedClient> &client = *it;
    client->receiveFromNetwork();
    messageBroker->dispatchMessages();
    it = client->isDisconnected() ? connectedClients.erase(it) : std::next(it);
  }
}

std::optional<std::unique_ptr<ClientSocket>> Server::acceptConnection() {
  int socket;
  struct sockaddr_in clientAddr {};
  socklen_t clientAddrLen{sizeof(clientAddr)};

  if ((socket = accept(serverSocket->getInternalSocket(), (struct sockaddr *) &clientAddr, &clientAddrLen)) == -1) {
#ifdef _WIN32
    int error = WSAGetLastError();
    if (error != WSAEWOULDBLOCK) {
      throw std::runtime_error("Failed to accept connection. WSA Error " + error);
    } else {
      return std::nullopt;
    }
#else
    if (errno != EWOULDBLOCK && errno != EAGAIN) {
      throw std::runtime_error("Failed to accept connection");
    } else {
      return std::nullopt;
    }
#endif
  }
  return std::make_unique<ClientSocket>(socket, clientAddr);
}