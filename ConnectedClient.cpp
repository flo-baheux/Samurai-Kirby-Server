#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include <iostream>
#include <format>

#include "ConnectedClient.h"

const int BUFFER_SIZE = 1024;

ConnectedClient::ConnectedClient(ClientConnectionInfo clientConnectionInfo, RoomsManager *roomManager)
    : clientConnectionInfo{clientConnectionInfo}, roomManager{roomManager}, room{nullptr}
{
  receiveThread = std::thread(&receiveFromClient, this);
  sendThread = std::thread(&sendToClient, this);
};

int ConnectedClient::getSocket() const
{
  return clientConnectionInfo.socket;
};

char *ConnectedClient::getIP() const
{
  return inet_ntoa(clientConnectionInfo.addr.sin_addr);
};

int ConnectedClient::getPort() const
{
  return ntohs(clientConnectionInfo.addr.sin_port);
};

std::string_view ConnectedClient::getNickname() const
{
  return nickname;
};

void ConnectedClient::receiveFromClient()
{
  std::cout << "handling client communication..." << std::endl;
  // Handle data
  char buffer[BUFFER_SIZE];
  int bytesRead;

  while ((bytesRead = recv(clientConnectionInfo.socket, buffer, sizeof(buffer), 0)) > 0)
  {
    std::string message(buffer, bytesRead);

    std::cout << std::format("Received [{}]", message) << std::endl;
    if (message.find("join:") != std::string::npos)
    {
      nickname = message.substr(message.find(':') + 1);
      // TODO: Get argument for room difficulty
      room = roomManager->joinAvailableRoom(this);
    }
    else if (message.find("unready") != std::string::npos)
    {
      isReady = false;
      room->playerNotifyReadyStatus(this, isReady);
    }
    else if (message.find("ready") != std::string::npos)
    {
      isReady = true;
      room->playerNotifyReadyStatus(this, isReady);
    }
    else if (message.find("canPlay") != std::string::npos)
    {
      canPlay = true;
    }
    else if (message.find("pressedButton:") != std::string::npos)
    {
      // Only works the first time this event is received
      if (timer == 0)
      {
        buttonPressed = message.substr(message.find(':') + 1, 1)[0];
        timer = std::stoi(message.substr(message.find("-") + 1));
        room->playerNotifyPressedButton(this);
      }
    }
    else if (message.find("wantsToReplay") != std::string::npos)
    {
      wantsToReplay = true;
    }
  }

  if (bytesRead == 0)
  {
    // Connection closed by the client
    std::cout << "Connection closed by client" << std::endl;
  }
  else if (bytesRead == SOCKET_ERROR)
  {
    perror("Error receiving data");
#ifdef _WIN32
    std::cerr << "Error code: " << WSAGetLastError() << std::endl;
#endif
  }

#ifdef _WIN32
  closesocket(clientConnectionInfo.socket);
#else
  close(clientConnectionInfo.socket);
#endif

  stopThreads = true;
  hasBeenDisconnected = true;
  room->playerNotifyDisconnect(this);
}

void ConnectedClient::sendMessage(const std::string &message)
{
  std::unique_lock<std::mutex> lock(mutexQueue);
  messageQueue.push(message);
  condition.notify_one();
}

void ConnectedClient::sendToClient()
{
  while (!stopThreads)
  {
    std::unique_lock<std::mutex> lock(mutexQueue);
    condition.wait(lock, [this]
                   { return !messageQueue.empty() || stopThreads; });

    if (stopThreads)
      break;
    std::string message = messageQueue.front();
    messageQueue.pop();
    lock.unlock(); // Unlock before sending to avoid blocking other threads

    std::cout << "Sending [" << message << "]" << std::endl;
    message = message + "\n";
    send(clientConnectionInfo.socket, message.c_str(), message.size(), 0);
  }
}

bool ConnectedClient::isPlayerReady() const
{
  return isReady;
}

bool ConnectedClient::playerCanPlay() const
{
  return canPlay;
}

int ConnectedClient::getTimer() const
{
  return timer;
}

char ConnectedClient::getButtonPressed() const
{
  return buttonPressed;
}

bool ConnectedClient::playerWantsToReplay() const
{
  return wantsToReplay;
}

void ConnectedClient::resetGameData()
{
  isReady = true;
  canPlay = false;
  wantsToReplay = false;
  timer = 0;
  buttonPressed = '\0';
}

bool ConnectedClient::isDisconnected() const
{
  return hasBeenDisconnected;
}

ConnectedClient::~ConnectedClient()
{
  stopThreads = true;

  condition.notify_all();

  if (receiveThread.joinable())
    receiveThread.join();

  if (sendThread.joinable())
    sendThread.join();
};
