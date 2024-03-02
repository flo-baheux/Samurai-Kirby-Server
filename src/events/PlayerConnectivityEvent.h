#ifndef PLAYERCONNECTIVITYEVENT
#define PLAYERCONNECTIVITYEVENT

#include <string>
#include <iostream>
#include <vector>

#include "GameDifficulty.h"

enum PlayerConnectivityEventType
{
  CONNECT,
  DISCONNECT,
};

class PlayerConnectivityEvent
{
protected:
  explicit PlayerConnectivityEvent(PlayerConnectivityEventType eventType)
      : eventType{eventType} {};

public:
  virtual ~PlayerConnectivityEvent(){};
  const PlayerConnectivityEventType eventType;
};

class PlayerConnectEvent : public PlayerConnectivityEvent
{
public:
  PlayerConnectEvent(int playerId)
      : PlayerConnectivityEvent(PlayerConnectivityEventType::CONNECT),
        playerId{playerId} {};

  const int playerId;
};

class PlayerDisconnectEvent : public PlayerConnectivityEvent
{
public:
  PlayerDisconnectEvent(int playerId)
      : PlayerConnectivityEvent(PlayerConnectivityEventType::DISCONNECT),
        playerId{playerId} {};

  const int playerId;
};

#endif // PLAYERCONNECTIVITYEVENT