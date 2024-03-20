#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include <format>
#include <algorithm>
#include <cctype>
#include <iostream>

#include "ConnectedClient.h"
#include "GameDifficulty.h"
#include "Serializer.h"

const int BUFFER_SIZE = 1024;

ConnectedClient::ConnectedClient(std::unique_ptr<ClientSocket> clientSocket, NetworkMessageBroker &messageBroker, int id)
    : clientSocket{std::move(clientSocket)},
      messageBroker{messageBroker},
      id{id}
{
  ConnectedClient::messageBroker.publishPlayerConnectivityMessage(PlayerConnectivityMessage{CONNECT, id});
  ConnectedClient::messageBroker.subscribeToGameplayMessagesForPlayer(
      id,
      [this](std::shared_ptr<GameplayMessage> message)
      { handleGameplayMessage(message); });
};

char *ConnectedClient::getIP() const
{
  return clientSocket->getIP();
};

int ConnectedClient::getPort() const
{
  return clientSocket->getPort();
};

int ConnectedClient::getId() const
{
  return id;
}

void ConnectedClient::receiveFromNetwork()
{
  // Handle data
  char buffer[BUFFER_SIZE];
  int bytesRead = recv(clientSocket->getInternalSocket(), buffer, sizeof(buffer), 0);

  if (bytesRead > 0)
  {
    std::string message(buffer, bytesRead);

    std::cout << "Received [" << message << "]" << std::endl;
    if (message.find("join:") != std::string::npos)
    {
      std::string arguments = message.substr(message.find(":") + 1);
      int separatorIndex = arguments.find(",");
      std::string nickname = arguments.substr(0, separatorIndex);
      std::string difficultyAsString = arguments.substr(separatorIndex + 1);
      // TODO: Send as enum
      std::transform(difficultyAsString.begin(), difficultyAsString.end(), difficultyAsString.begin(), [](unsigned char c)
                     { return std::tolower(c); });
      GameDifficulty difficulty = gameDifficultyStringToEnum.at(difficultyAsString);
      messageBroker.publishPlayerActionMessage(id, std::make_shared<JoinGamePlayerActionMessage>(nickname, difficulty));
    }
    else if (message.find("unready") != std::string::npos)
    {
      messageBroker.publishPlayerActionMessage(id, std::make_shared<SetReadyStatePlayerActionMessage>(false));
    }
    else if (message.find("ready") != std::string::npos)
    {
      messageBroker.publishPlayerActionMessage(id, std::make_shared<SetReadyStatePlayerActionMessage>(true));
    }
    else if (message.find("canPlay") != std::string::npos)
    {
      messageBroker.publishPlayerActionMessage(id, std::make_shared<NotifyCanPlayPlayerActionMessage>());
    }
    else if (message.find("pressedButton:") != std::string::npos)
    {
      int buttonPressedEnumValueAsInt = std::stoi(message.substr(message.find(':') + 1, 1));
      int timeToPress = std::stoi(message.substr(message.find(",") + 1));
      GameplayInput buttonPressed = (GameplayInput)buttonPressedEnumValueAsInt;
      messageBroker.publishPlayerActionMessage(id, std::make_shared<InputPlayerActionMessage>(buttonPressed, timeToPress));
    }
    else if (message.find("wantsToReplay") != std::string::npos)
    {
      messageBroker.publishPlayerActionMessage(id, std::make_shared<SetReplayReadyStatePlayerActionMessage>(true));
    }
    else if (message.find("leave") != std::string::npos)
    {
      messageBroker.publishPlayerActionMessage(id, std::make_shared<LeaveGamePlayerActionMessage>());
    }
    else if (message.find("disconnect") != std::string::npos)
    {
      messageBroker.publishPlayerConnectivityMessage(PlayerConnectivityMessage{DISCONNECT, id});
    }
  }

  if (bytesRead == 0)
  {
    std::cout << "Connection closed by client" << std::endl;
    hasBeenDisconnected = true;
    messageBroker.publishPlayerConnectivityMessage(PlayerConnectivityMessage{DISCONNECT, id});
  }

  else if (bytesRead == -1)
  {
#ifdef _WIN32
    int error = WSAGetLastError();
    if (error != WSAEWOULDBLOCK)
    {
      messageBroker.publishPlayerConnectivityMessage(PlayerConnectivityMessage{DISCONNECT, id});

      throw std::runtime_error("Failed to receive data. WSA Error " + error);
    }
#else
    if (errno != EWOULDBLOCK && errno != EAGAIN)
    {
      messageBroker.publishPlayerConnectivityMessage(PlayerConnectMessage{id});

      throw std::runtime_error("Failed to receive data");
    }
#endif
  }
}

void ConnectedClient::handleGameplayMessage(std::shared_ptr<GameplayMessage> message)
{
  if (!isDisconnected())
  {
    clientSocket->send(Serializer::serialize(*message));
  }
}

bool ConnectedClient::isDisconnected() const
{
  return hasBeenDisconnected;
}
