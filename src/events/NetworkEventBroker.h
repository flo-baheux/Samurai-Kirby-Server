#ifndef NETWORKEVENTHUB_H
#define NETWORKEVENTHUB_H

#include <vector>

#include "EventHub.h"

class NetworkEventBroker
{
public:
  NetworkEventBroker(EventHub &eventHub) : eventHub{eventHub} {};

  NetworkEventBroker(NetworkEventBroker const &) = delete;
  void operator=(NetworkEventBroker const &) = delete;

  void publishPlayerConnectivityEvent(PlayerConnectivityEvent event)
  {
    eventHub.playerConnectivityEvents.push(event);
  }

  void publishPlayerActionEvent(int playerId, PlayerActionEvent event)
  {
    eventHub.playerActionEventsByPlayerId[playerId].push(event);
  }

  std::vector<GameplayEvent> receiveGameplayEvents(int playerId)
  {
    std::vector<GameplayEvent> events{};
    while (!eventHub.gameplayEventsByPlayerId[playerId].empty())
    {
      events.push_back(eventHub.gameplayEventsByPlayerId[playerId].front());
      eventHub.gameplayEventsByPlayerId[playerId].pop();
    }
    return events;
  }

private:
  EventHub &eventHub;
};

#endif // NETWORKEVENTHUB_H
