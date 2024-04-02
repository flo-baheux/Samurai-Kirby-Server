#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include "ConnectedClient.h"
#include "Serializer.h"

const int BUFFER_SIZE = 1024;

ConnectedClient::ConnectedClient(std::unique_ptr<ClientSocket> clientSocket, NetworkMessageBroker &messageBroker, int id)
  : clientSocket{std::move(clientSocket)},
  messageBroker{messageBroker},
  id{id} {
  ConnectedClient::messageBroker.publishPlayerConnectivityMessage(PlayerConnectivityMessage{CONNECT, id});
  ConnectedClient::messageBroker.subscribeToGameplayMessagesForPlayer(
    id,
    [this](std::shared_ptr<GameplayMessage> message) { handleGameplayMessage(message); });
};

char *ConnectedClient::getIP() const {
  return clientSocket->getIP();
};

int ConnectedClient::getPort() const {
  return clientSocket->getPort();
};

int ConnectedClient::getId() const {
  return id;
}

void ConnectedClient::receiveFromNetwork() {
  char buffer[BUFFER_SIZE];
  int bytesRead = recv(clientSocket->getInternalSocket(), buffer, sizeof(buffer), 0);

  if (bytesRead > 0) {
    std::string message(buffer, bytesRead);
    try {
      messageBroker.publishPlayerActionMessage(id, Serializer::deserializePlayerActionMessage(message));
    } catch (DeserializeNotHandledException notHandledException) {
      // TODO: Logger
      return;
    } catch (DeserializeException deserializeException) {
      // TODO: Logger
      return;
    }
  }

  if (bytesRead == 0) {
    std::cout << "Connection closed by client" << std::endl;
    hasBeenDisconnected = true;
    messageBroker.publishPlayerConnectivityMessage(PlayerConnectivityMessage{DISCONNECT, id});
  }

  else if (bytesRead == -1) {
#ifdef _WIN32
    int error = WSAGetLastError();
    if (error != WSAEWOULDBLOCK) {
      messageBroker.publishPlayerConnectivityMessage(PlayerConnectivityMessage{DISCONNECT, id});

      throw std::runtime_error("Failed to receive data. WSA Error " + error);
    }
#else
    if (errno != EWOULDBLOCK && errno != EAGAIN) {
      messageBroker.publishPlayerConnectivityMessage(PlayerConnectivityMessage{DISCONNECT, id});

      throw std::runtime_error("Failed to receive data");
    }
#endif
  }
}

void ConnectedClient::handleGameplayMessage(std::shared_ptr<GameplayMessage> message) {
  if (!isDisconnected()) {
    try {
      clientSocket->send(Serializer::serializeGameplayMessage(*message));
    } catch (SerializeNotHandledException notHandledException) {
      // TODO: Logger
      return;
    }
  }
}

bool ConnectedClient::isDisconnected() const {
  return hasBeenDisconnected;
}

ConnectedClient::~ConnectedClient() {
  messageBroker.unsubscribeToGameplayMessagesForPlayer(id);
}
