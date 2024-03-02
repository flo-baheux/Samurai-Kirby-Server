#ifndef GAMEPLAYEVENTBROKER_H
#define GAMEPLAYEVENTBROKER_H

#include <vector>

#include "EventHub.h"

class GameplayEventBroker
{
public:
  GameplayEventBroker(EventHub &eventHub) : eventHub{eventHub} {};

private:
  EventHub &eventHub;

  void publishPlayerEvent(int playerId, GameplayEvent event)
  {
    eventHub.gameplayEventsByPlayerId[playerId].push(event);
  }

  std::vector<PlayerActionEvent> getPlayerActionEvents(int playerId)
  {
    std::vector<PlayerActionEvent> events{};
    while (!eventHub.playerActionEventsByPlayerId[playerId].empty())
    {
      events.push_back(eventHub.playerActionEventsByPlayerId[playerId].front());
      eventHub.playerActionEventsByPlayerId[playerId].pop();
    }
    return events;
  }
};

#endif // GAMEPLAYEVENTBROKER_H
