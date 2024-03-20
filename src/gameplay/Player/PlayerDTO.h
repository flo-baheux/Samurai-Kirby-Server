#pragma once

#include <string>
#include <optional>

#include "PlayerAssignment.h"
#include "GameplayInput.h"

struct PlayerLobbyDTO
{
  const std::string nickname;
  const PlayerAssignment assignment;
  const bool isReady;
};

struct PlayerGameResultDTO
{
  const PlayerAssignment assignment;
  std::optional<GameplayInput> inputPressed;
  std::optional<int> timeToPress;
};