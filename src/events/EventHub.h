#ifndef EVENTHUB_H
#define EVENTHUB_H

#include <unordered_map>
#include <queue>

#include "PlayerConnectivityEvent.h"
#include "PlayerActionEvents.h"
#include "GameplayEvents.h"

class EventHub
{
public:
  ~EventHub(){};
  EventHub(EventHub const &) = delete;
  void operator=(EventHub const &) = delete;

  std::unordered_map<int, std::queue<PlayerActionEvent>> playerActionEventsByPlayerId;
  std::unordered_map<int, std::queue<GameplayEvent>> gameplayEventsByPlayerId;

  std::queue<PlayerConnectivityEvent> playerConnectivityEvents;

private:
  EventHub(){};

  friend class Server;
};

#endif // EVENTHUB_H
