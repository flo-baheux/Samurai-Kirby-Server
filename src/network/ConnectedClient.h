#ifndef CONNECTEDCLIENT_H
#define CONNECTEDCLIENT_H

#include <memory>

#include "ClientSocket.h"
#include "NetworkEventBroker.h"

class RoomsManager;
class GameplayRoom;

class ConnectedClient
{
public:
  ConnectedClient(std::unique_ptr<ClientSocket>, std::unique_ptr<NetworkEventBroker>, int id);
  ~ConnectedClient(){};

  ConnectedClient() = delete;
  ConnectedClient(const ConnectedClient &) = delete;
  ConnectedClient &operator=(const ConnectedClient &) = delete;

  char *getIP() const;
  int getPort() const;
  int getId() const;

  void sendMessage(const std::string &message);

  void receiveFromNetwork();
  void sendToNetwork();
  bool isDisconnected() const;

private:
  std::unique_ptr<ClientSocket> clientSocket;
  std::unique_ptr<NetworkEventBroker> eventBroker;

  bool hasBeenDisconnected{false};

  int id{};
};

#endif // CONNECTEDCLIENT_H
