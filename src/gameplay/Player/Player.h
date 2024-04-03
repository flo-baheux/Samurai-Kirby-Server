#pragma once

#include <string>
#include <optional>

#include "PlayerAssignment.h"
#include "GameplayInput.h"
#include "GameplayChrono.h"
#include "PlayerDTO.h"

class Player {
  friend class Room;

private:
  Player(int id, std::string nickname, PlayerAssignment assignment)
    : id{id},
    nickname{nickname},
    assignment{assignment} {};

public:
  int id;
  std::string nickname;
  PlayerAssignment assignment;
  bool isReady{false};
  bool canPlay{false};
  bool isReadyToReplay{false};
  std::optional<GameplayInput> inputPressed;
  std::optional<int> timeToPress;
  std::optional<ChronoTimePoint> inputPressedAtServerTime;

  void resetToReplay();
  const PlayerLobbyDTO buildLobbyDTO() const;
  const PlayerGameResultDTO buildGameoverDTO() const;
};
