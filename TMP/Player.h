#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <iostream>
// #include "MessageHub.h"

class Player
{
public:
  Player(int id) : id{id}
  {
    std::cout << "Player created" << std::endl;
  };
  ~Player()
  {
    std::cout << "Player deleted" << std::endl;
  };

  Player() = delete;
  Player(const Player &) = delete;
  Player &operator=(const Player &) = delete;

  // const std::string getNickname() const;
  // const bool isReady() const;
  // const bool canPlay() const;
  // const bool hasPlayed() const;
  // const bool wantsToReplay() const;

  // will be more specialized later

  // int getTimer() const;
  // char getButtonPressed() const;
  // void resetGameData();

  // void processMessages();

private:
  const int id;
  // std::string nickname{};
  // bool _isReady{false};
  // bool _canPlay{false};
  // bool _hasPlayed{false};
  // bool _wantsToReplay{false};

  // char buttonPressed;
  // int timer{0};
};

#endif // PLAYER_H
