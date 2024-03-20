#include "Room.h"

void Room::tick()
{
  switch (gameplayState)
  {
  case WAITING_PLAYERS:
    handleEveryoneReady();
    break;
  case STARTED:
    handleEveryoneCanPlay();
    break;
  case EXPECTING_INPUT:
    handleOnePlayerPlayed();
    handleEveryonePlayed();
    handleTimeout();
    break;
  case OVER:
    handleReplay();
    break;
  }
}

void Room::handlePlayerMessages(int playerId, std::shared_ptr<PlayerActionMessage> message)
{
  std::optional<Player *> player = getPlayerById(playerId);
  if (!player.has_value())
  {
    // log warning
    return;
  }

  switch (message->messageType)
  {
  case SET_READY_STATE:
    return handlePlayerSetReadyStateMessage(player.value(), std::dynamic_pointer_cast<SetReadyStatePlayerActionMessage>(message));
  case NOTIFY_CAN_PLAY:
    return handlePlayerNotifyCanPlayMessage(player.value(), std::dynamic_pointer_cast<NotifyCanPlayPlayerActionMessage>(message));
  case PLAYER_INPUT:
    return handlePlayerInputMessage(player.value(), std::dynamic_pointer_cast<InputPlayerActionMessage>(message));
  case SET_REPLAY_READY_STATE:
    return handlePlayerSetReplayReadyStateMessage(player.value(), std::dynamic_pointer_cast<SetReplayReadyStatePlayerActionMessage>(message));
  case LEAVE_GAME:
    return handlePlayerLeaveGameMessage(player.value(), std::dynamic_pointer_cast<LeaveGamePlayerActionMessage>(message));
  default:
    break;
  }
};

void Room::handleEveryoneReady()
{
  if (gameplayState != WAITING_PLAYERS || !isEveryoneReady())
    return;

  gameplayState = STARTED;
  broadcastGameplayMessage(std::make_shared<AllReadyGameStartingMessage>());
};

void Room::handleEveryoneCanPlay()
{
  if (gameplayState != STARTED || !everyoneCanPlay())
    return;

  if (!everyoneCanPlayAt.has_value())
    everyoneCanPlayAt = getTimeNow();

  if (getSecondsSince(everyoneCanPlayAt.value()) >= secondsBeforeInputRequest)
  {
    gameplayState = EXPECTING_INPUT;
    readyForInputAt = getTimeNow();
    expectedInput = getRandomGameplayInput();
    broadcastGameplayMessage(std::make_shared<GameExpectingInputMessage>(expectedInput));
  }
};

void Room::handleTimeout()
{
  if (gameplayState != EXPECTING_INPUT)
    return;

  if (noOnePlayed() && getSecondsSince(readyForInputAt.value()) > NO_INPUT_TIMEOUT_S)
  {
    // TODO: MESSAGE
    broadcastGameplayMessage(std::make_shared<GameOverDrawMessage>(buildGameResultDTO()));
    gameplayState = OVER;
  }
};

void Room::handleOnePlayerPlayed()
{
  if (gameplayState != STARTED && gameplayState != EXPECTING_INPUT)
    return;

  if (everyonePlayed() || noOnePlayed())
    return;

  PlayerAssignment whoPlayed = players[P1].value().inputPressed.has_value() ? P1 : P2;
  PlayerAssignment whoDidNotPlay = whoPlayed == P1 ? P2 : P1;

  // player pressed too early - failed
  if (gameplayState != EXPECTING_INPUT)
  {
    gameplayState = OVER;
    broadcastGameplayMessage(std::make_shared<GameOverEarlyInputMessage>(buildGameResultDTO(whoDidNotPlay)));
    return;
  }

  // pressed the wrong button
  if (players[whoPlayed].value().inputPressed.value() != expectedInput)
  {
    gameplayState = OVER;
    broadcastGameplayMessage(std::make_shared<GameOverWrongInputMessage>(buildGameResultDTO(whoDidNotPlay)));
    return;
  }

  // timeout
  const int timeSincePlayerPlayed = getSecondsSince(players[whoPlayed].value().inputPressedAtServerTime.value());
  if (timeSincePlayerPlayed >= ONE_PLAYER_ONLY_TIMEOUT_S)
  {
    gameplayState = OVER;
    broadcastGameplayMessage(std::make_shared<GameOverWinMessage>(buildGameResultDTO(whoPlayed)));
  }
};

void Room::handleEveryonePlayed()
{
  if (gameplayState != EXPECTING_INPUT)
    return;

  if (!everyonePlayed())
    return;

  gameplayState = OVER;
  ChronoTimePoint &P1PressedAt = players[P1].value().inputPressedAtServerTime.value();
  ChronoTimePoint &P2PressedAt = players[P2].value().inputPressedAtServerTime.value();
  if (P1PressedAt < P2PressedAt)
  {
    broadcastGameplayMessage(std::make_shared<GameOverWinMessage>(buildGameResultDTO(P1)));
  }
  else if (P2PressedAt < P1PressedAt)
  {
    broadcastGameplayMessage(std::make_shared<GameOverWinMessage>(buildGameResultDTO(P2)));
  }
  else
  {
    broadcastGameplayMessage(std::make_shared<GameOverDrawMessage>(buildGameResultDTO()));
  }
};

void Room::handleReplay()
{
  if (gameplayState != OVER || !everyoneWantsToReplay())
    return;

  // TODO: CODE

  // players[P1].reset();
  // players[P2].reset();
};

void Room::handlePlayerSetReadyStateMessage(Player *player, std::shared_ptr<SetReadyStatePlayerActionMessage> message)
{
  if (gameplayState != WAITING_PLAYERS)
  {
    // TODO: Logger
    std::cout << "WARNING - Unexpectedly received SetReadStatePlayerMessage" << std::endl;
    return;
  }
  player->isReady = message->isReady;
  broadcastGameplayMessage(std::make_shared<PlayerReadyStateChangedMessage>(player->assignment, player->isReady));
}

void Room::handlePlayerNotifyCanPlayMessage(Player *player, std::shared_ptr<NotifyCanPlayPlayerActionMessage> message)
{
  player->canPlay = true;
}

void Room::handlePlayerInputMessage(Player *player, std::shared_ptr<InputPlayerActionMessage> message)
{
  player->inputPressed = message->inputPressed;
  player->timeToPress = message->timeToPress;
  player->inputPressedAtServerTime = getTimeNow();
}

void Room::handlePlayerSetReplayReadyStateMessage(Player *player, std::shared_ptr<SetReplayReadyStatePlayerActionMessage>(message))
{
  player->isReadyToReplay = message->isReadyToReplay;
  broadcastGameplayMessage(std::make_shared<PlayerReplayReadyStateChangedMessage>(player->assignment, player->isReadyToReplay));
}

void Room::handlePlayerLeaveGameMessage(Player *player, std::shared_ptr<LeaveGamePlayerActionMessage>(message))
{
  // TODO
}

void Room::addPlayer(int id, std::string nickname)
{
  const std::optional<PlayerAssignment> playerAssignment = getAvailableAssignment();
  if (!playerAssignment.has_value())
  {
    // TODO: Logger
    std::cout << "Error: Cannot add player - room is full." << std::endl;
    return;
  }

  Player player{id, nickname, playerAssignment.value()};
  players[player.assignment] = player;

  PlayerAssignment otherAssignment = player.assignment == P1 ? P2 : P1;
  std::optional<PlayerLobbyDTO> otherPlayerLobbyDTO = players[otherAssignment].has_value() ? std::optional(players[otherAssignment].value().buildLobbyDTO()) : std::nullopt;
  messageBroker.publishGameplayMessage(id, std::make_shared<RoomJoinedMessage>(playerAssignment.value(), otherPlayerLobbyDTO));
  if (players[otherAssignment].has_value())
    messageBroker.publishGameplayMessage(players[otherAssignment].value().id, std::make_shared<PlayerJoinedRoomMessage>(player.buildLobbyDTO()));

  messageBroker.subscribeToPlayerActionMessages(id,
                                                [this](int playerId, std::shared_ptr<PlayerActionMessage> e)
                                                { handlePlayerMessages(playerId, e); });
}

inline bool const Room::isFull() const
{
  return players[P1].has_value() && players[P2].has_value();
}

inline bool Room::isEveryoneReady() const
{
  return isFull() && players[P1].value().isReady && players[P2].value().isReady;
}

inline bool Room::everyoneCanPlay() const
{
  return isFull() && players[P1].value().canPlay && players[P2].value().canPlay;
}

inline bool Room::everyonePlayed() const
{
  return isFull() && players[P1].value().inputPressed.has_value() && players[P2].value().inputPressed.has_value();
}

inline bool Room::noOnePlayed() const
{
  return isFull() && !players[P1].value().inputPressed.has_value() && !players[P2].value().inputPressed.has_value();
}

inline bool Room::everyoneWantsToReplay() const
{
  return isFull() && players[P1].value().isReadyToReplay && players[P2].value().isReadyToReplay;
}

void Room::broadcastGameplayMessage(std::shared_ptr<GameplayMessage> message) const
{
  if (players[P1].has_value())
    messageBroker.publishGameplayMessage(players[P1].value().id, message);
  if (players[P2].has_value())
    messageBroker.publishGameplayMessage(players[P2].value().id, message);
}

std::optional<Player *> Room::getPlayerById(int id)
{
  if (players[P1].has_value() && players[P1].value().id == id)
    return &players[P1].value();
  else if (players[P2].has_value() && players[P2].value().id == id)
    return &players[P2].value();
  else
    return std::nullopt;
}

const std::optional<const PlayerAssignment> Room::getAvailableAssignment() const
{
  if (!players[P1].has_value())
    return PlayerAssignment::P1;
  else if (!players[P2].has_value())
    return PlayerAssignment::P2;
  else
    return std::nullopt;
}

const GameplayInput Room::getRandomGameplayInput() const
{
  std::vector<GameplayInput> inputs = {SOUTH, EAST, WEST, NORTH};
  switch (difficulty)
  {
  default:
  case EASY:
    return inputs[0]; // A
  case MEDIUM:
    return inputs[getRandomValueInRange(0, 1)]; // A / B
  case HARD:
    return inputs[getRandomValueInRange(0, inputs.size() - 1)]; // A / B / X / Y
  }
}

const GameResultDTO Room::buildGameResultDTO(std::optional<PlayerAssignment> winningPlayer) const
{
  return {
      winningPlayer,
      players[P1].value().buildGameoverDTO(),
      players[P2].value().buildGameoverDTO(),
  };
}
