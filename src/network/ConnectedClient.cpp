#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include <algorithm>
#include <cctype>
#include <iostream>

#include "ConnectedClient.h"
#include "GameDifficulty.h"

const int BUFFER_SIZE = 1024;

ConnectedClient::ConnectedClient(std::unique_ptr<ClientSocket> clientSocket, std::unique_ptr<NetworkEventBroker> eventBroker, int id)
    : clientSocket{std::move(clientSocket)},
      eventBroker{std::move(eventBroker)},
      id{id}
{
  eventBroker->publishPlayerConnectivityEvent(PlayerConnectEvent{id});
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
      std::string nickname = arguments.substr(0, separatorIndex - 1);
      std::string difficultyAsString = arguments.substr(separatorIndex + 1);
      std::transform(difficultyAsString.begin(), difficultyAsString.end(), difficultyAsString.begin(), [](unsigned char c)
                     { return std::tolower(c); });

      // WARNING: This is NOT SAFE AT ALL - ok for now until the game works
      GameDifficulty difficulty = gameDifficultyStringToEnum.find(difficultyAsString)->second;
      eventBroker->publishPlayerActionEvent(id, JoinGamePlayerActionEvent(nickname, difficulty));
    }
    else if (message.find("unready") != std::string::npos)
    {
      eventBroker->publishPlayerActionEvent(id, SetReadyStatePlayerActionEvent(false));
      // room->playerNotifyReadyStatus(this, isReady);
    }
    else if (message.find("ready") != std::string::npos)
    {
      eventBroker->publishPlayerActionEvent(id, SetReadyStatePlayerActionEvent(true));
      // room->playerNotifyReadyStatus(this, isReady);
    }
    else if (message.find("canPlay") != std::string::npos)
    {
      eventBroker->publishPlayerActionEvent(id, SetReadyStatePlayerActionEvent(true));
      // canPlay = true;
    }
    else if (message.find("pressedButton:") != std::string::npos)
    {
      char buttonPressedAsChar = std::toupper(message.substr(message.find(':') + 1, 1)[0]);
      int timeToPress = std::stoi(message.substr(message.find(",") + 1));
      ButtonPressed buttonPressed;
      switch (buttonPressedAsChar)
      {
      case 'A':
        buttonPressed = ButtonPressed::A;
      case 'B':
        buttonPressed = ButtonPressed::B;
      case 'X':
        buttonPressed = ButtonPressed::X;
      case 'Y':
        buttonPressed = ButtonPressed::Y;
      }
      eventBroker->publishPlayerActionEvent(id, PressedButtonPlayerActionEvent(buttonPressed, timeToPress));

      //   // Only works the first time this event is received
      //   if (timer == 0)
      //   {
      //     buttonPressed = message.substr(message.find(':') + 1, 1)[0];
      //     timer = std::stoi(message.substr(message.find(",") + 1));
      //     room->playerNotifyPressedButton(this);
      //   }
    }
    else if (message.find("wantsToReplay") != std::string::npos)
    {
      // wantsToReplay = true;
      eventBroker->publishPlayerActionEvent(id, SetReplayReadyStatePlayerActionEvent(true));
    }
    else if (message.find("leave") != std::string::npos)
    {
      eventBroker->publishPlayerActionEvent(id, LeaveGamePlayerActionEvent());
    }
    else if (message.find("disconnect") != std::string::npos)
    {
      eventBroker->publishPlayerConnectivityEvent(PlayerConnectEvent{id});
    }
  }

  if (bytesRead == 0)
  {
    // Connection closed by the client
    std::cout << "Connection closed by client" << std::endl;
    hasBeenDisconnected = true;
    eventBroker->publishPlayerConnectivityEvent(PlayerConnectEvent{id});
  }

  else if (bytesRead == -1)
  {
#ifdef _WIN32
    int error = WSAGetLastError();
    if (error != WSAEWOULDBLOCK)
    {
      eventBroker->publishPlayerConnectivityEvent(PlayerConnectEvent{id});

      throw std::runtime_error("Failed to receive data. WSA Error " + error);
    }
#else
    if (errno != EWOULDBLOCK && errno != EAGAIN)
    {
      eventBroker->publishPlayerConnectivityEvent(PlayerConnectEvent{id});

      throw std::runtime_error("Failed to receive data");
    }
#endif
  }
}

void ConnectedClient::sendToNetwork()
{
  std::vector<GameplayEvent> events = eventBroker->receiveGameplayEvents(id);

  for (GameplayEvent &e : events)
  {
    std::string message = "TOTO";
    std::cout << "Sending [" << message << "]" << std::endl;
    message += "\n";
    clientSocket->send(message);
  }
}

// bool ConnectedClient::isPlayerReady() const
// {
//   return isReady;
// }

// bool ConnectedClient::playerCanPlay() const
// {
//   return canPlay;
// }

// int ConnectedClient::getTimer() const
// {
//   return timer;
// }

// char ConnectedClient::getButtonPressed() const
// {
//   return buttonPressed;
// }

// bool ConnectedClient::playerWantsToReplay() const
// {
//   return wantsToReplay;
// }

// void ConnectedClient::resetGameData()
// {
//   isReady = true;
//   canPlay = false;
//   wantsToReplay = false;
//   timer = 0;
//   buttonPressed = '\0';
// }

bool ConnectedClient::isDisconnected() const
{
  return hasBeenDisconnected;
}
