#ifndef MESSAGEHUB_H
#define MESSAGEHUB_H

#include <queue>
#include <memory>
#include <unordered_map>

#include "PlayerConnectivityMessage.h"
#include "PlayerActionMessages.h"
#include "GameplayMessages.h"

class MessageHub
{
public:
  ~MessageHub(){};
  MessageHub(MessageHub const &) = delete;
  void operator=(MessageHub const &) = delete;

  std::unordered_map<int, std::queue<std::shared_ptr<PlayerActionMessage>>> playerActionMessagesByPlayerId;
  std::unordered_map<int, std::queue<std::shared_ptr<GameplayMessage>>> gameplayMessagesByPlayerId;

  std::queue<PlayerConnectivityMessage> playerConnectivityMessages;

private:
  MessageHub(){};

  friend class Server;
};

#endif // MESSAGEHUB_H