#ifndef CONNECTEDCLIENT_H
#define CONNECTEDCLIENT_H

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>

#include "RoomsManager.h"
#include "GameplayRoom.h"

class RoomsManager;
class GameplayRoom;
struct ClientConnectionInfo
{
  int socket;
  struct sockaddr_in addr;
};

class ConnectedClient
{
public:
  ConnectedClient(ClientConnectionInfo, RoomsManager *);
  ~ConnectedClient();

  ConnectedClient() = delete;
  ConnectedClient(const ConnectedClient &) = delete;
  ConnectedClient &operator=(const ConnectedClient &) = delete;

  int getSocket() const;
  char *getIP() const;
  int getPort() const;
  std::string_view getNickname() const;

  // will be more specialized later
  void sendMessage(const std::string &message);
  bool isPlayerReady() const;
  bool playerCanPlay() const;
  bool playerHasPlayed() const;
  bool playerWantsToReplay() const;
  int getTimer() const;
  char getButtonPressed() const;
  void resetGameData();

  bool isDisconnected() const;

private:
  ClientConnectionInfo clientConnectionInfo{};

  // thread
  std::thread receiveThread;
  std::thread sendThread;
  std::queue<std::string> messageQueue;
  std::mutex mutexQueue;
  std::condition_variable condition;
  bool stopThreads{false};
  bool hasBeenDisconnected{false};
  char buttonPressed;
  int timer{0};

  RoomsManager *roomManager{};
  GameplayRoom *room{};
  std::string nickname{};
  bool isReady{false};
  bool canPlay{false};

  bool wantsToReplay{false};

  void receiveFromClient();
  void sendToClient();
};

#endif // CONNECTEDCLIENT_H
