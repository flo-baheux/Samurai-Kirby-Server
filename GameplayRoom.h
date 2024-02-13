#ifndef GAMEPLAYROOM_H
#define GAMEPLAYROOM_H

#include <thread>

#include "ConnectedClient.h"
#include "GameDifficulty.h"

class ConnectedClient;
class GameplayRoom
{
public:
  GameplayRoom(GameDifficulty);
  ~GameplayRoom();
  GameplayRoom(const GameplayRoom &) = delete;
  GameplayRoom &operator=(const GameplayRoom &) = delete;

  bool addPlayer(ConnectedClient *);
  void playerNotifyReadyStatus(ConnectedClient *, bool isReady);
  void playerNotifyWantsToReplay(ConnectedClient *player);
  void playerNotifyDisconnect(ConnectedClient *player);
  void playerNotifyPressedButton(ConnectedClient *player);

  GameDifficulty getRoomDifficulty() const;

  bool isFull() const;
  bool isEmpty() const;

private:
  std::thread gameplayThread;
  bool stopThreads{false};

  enum GameplayState
  {
    WAITING_PLAYERS,
    CAN_START,
    STARTED,
    OVER,
  };

  static const int NO_INPUT_TIMEOUT_S{3};
  static const int ONE_PLAYER_ONLY_TIMEOUT_S{1};

  ConnectedClient *player1{nullptr};
  ConnectedClient *player2{nullptr};
  GameplayState gameState{WAITING_PLAYERS};
  GameDifficulty difficulty;
  char buttonToPress{'\0'};
  std::chrono::_V2::system_clock::time_point gameStartedAt{std::chrono::system_clock::time_point::min()};
  std::chrono::_V2::system_clock::time_point player1PressedAt{std::chrono::system_clock::time_point::min()};
  std::chrono::_V2::system_clock::time_point player2PressedAt{std::chrono::system_clock::time_point::min()};

  void handleGameplay();

  bool isEveryoneReady() const;
  bool everyoneCanPlay() const;
  bool everyonePlayed() const;
  bool everyoneWantsToReplay() const;

  void broadcastToPlayers(const std::string &message);
  char getRandomButtonToPress() const;

  bool gameReachedTimeout();

  void handleEveryoneReady();
  void handleEveryoneCanPlay();
  void handleTimeout();
  void handleOnePlayerPlayedTimeout();
  void handleEveryonePlayed();
  void handleReplay();
};

#endif