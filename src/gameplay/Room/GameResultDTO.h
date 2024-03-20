#pragma once

#include <optional>

#include "PlayerAssignment.h"
#include "PlayerDTO.h"

struct GameResultDTO
{
  std::optional<PlayerAssignment> winningPlayer;
  PlayerGameResultDTO player1;
  PlayerGameResultDTO player2;
};