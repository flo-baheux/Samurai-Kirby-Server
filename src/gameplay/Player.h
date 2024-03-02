#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "EventHub.h"

class Player
{
public:
  Player(int id) : id{id} {};
  ~Player(){};

  Player() = delete;
  Player(const Player &) = delete;
  Player &operator=(const Player &) = delete;

  std::string_view getNickname() const;

private:
  // will be more specialized later
  bool isPlayerReady() const;
  bool playerCanPlay() const;
  bool playerHasPlayed() const;
  bool playerWantsToReplay() const;
  int getTimer() const;
  char getButtonPressed() const;
  void resetGameData();
  bool isDisconnected() const;

  void processEvents();

private:
  const int id;

  bool hasBeenDisconnected{false};
  char buttonPressed;
  int timer{0};

  std::string nickname{};
  bool isReady{false};
  bool canPlay{false};
  bool wantsToReplay{false};
};

#endif // PLAYER_H
