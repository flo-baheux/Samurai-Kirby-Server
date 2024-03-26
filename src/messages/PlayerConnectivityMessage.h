#ifndef PLAYERCONNECTIVITYMESSAGE_H
#define PLAYERCONNECTIVITYMESSAGE_H

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
    : messageType{ messageType }, playerId{ playerId } {};

  const PlayerConnectivityMessageType messageType;
  const int playerId;
};

#endif // PLAYERCONNECTIVITYMESSAGE_H