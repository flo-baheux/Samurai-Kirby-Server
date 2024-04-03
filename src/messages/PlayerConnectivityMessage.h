#pragma once

#include <string>
#include <iostream>
#include <vector>

#include "GameDifficulty.h"

enum PlayerConnectivityMessageType {
  CONNECT,
  DISCONNECT,
};

class PlayerConnectivityMessage {
public:
  explicit PlayerConnectivityMessage(PlayerConnectivityMessageType messageType, int playerId)
    : messageType{messageType}, playerId{playerId} {};

  const PlayerConnectivityMessageType messageType;
  const int playerId;
};
