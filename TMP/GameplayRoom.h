#ifndef GAMEPLAYROOM_H
#define GAMEPLAYROOM_H

#include <thread>
#include <sstream>

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
  void playerNotifyLeftRoom(ConnectedClient *player);
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

  static const int NO_INPUT_TIMEOUT_S{5};
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

  void handleEveryoneReady();
  void handleEveryoneCanPlay();
  void handleTimeout();
  void handleOnePlayerPlayed();
  void handleEveryonePlayed();
  void handleReplay();

  class GameResultDTO
  {
    char player1ButtonPressed;
    int player1PressedInMs;
    int player1TotalWins;
    char player2ButtonPressed;
    int player2PressedInMs;
    int player2TotalWins;
    int winningPlayer; // 1 or 2

  public:
    GameResultDTO(
        char player1ButtonPressed,
        int player1PressedInMs,
        int player1TotalWins,
        char player2ButtonPressed,
        int player2PressedInMs,
        int player2TotalWins,
        int winningPlayer) : player1ButtonPressed{player1ButtonPressed},
                             player1PressedInMs{player1PressedInMs},
                             player1TotalWins{player1TotalWins},
                             player2ButtonPressed{player2ButtonPressed},
                             player2PressedInMs{player2PressedInMs},
                             player2TotalWins{player2TotalWins},
                             winningPlayer{winningPlayer} {};

    std::string serialize()
    {
      std::ostringstream serializedStream;

      serializedStream << "gameResult-"
                       << player1ButtonPressed << "," << player1PressedInMs << "," << player1TotalWins << ";"
                       << player2ButtonPressed << "," << player2PressedInMs << "," << player2TotalWins << ";"
                       << "player" << winningPlayer;

      return serializedStream.str();
    };
  };

  GameResultDTO buildGameResultDTO(int winningPlayerNb);
};

#endif