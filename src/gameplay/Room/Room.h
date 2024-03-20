#ifndef ROOM_H
#define ROOM_H

#include <iostream>
#include <optional>
#include <memory>

#include "GameDifficulty.h"
#include "PlayerAssignment.h"
#include "GameplayMessageBroker.h"
#include "Player.h"
#include "GameplayChrono.h"
#include "GameResultDTO.h"
#include "GetRandomValueInRange.h"

class Room
{
public:
  Room(GameplayMessageBroker &messageBroker, GameDifficulty difficulty = GameDifficulty::HARD)
      : messageBroker{messageBroker},
        difficulty{difficulty},
        secondsBeforeInputRequest{getRandomValueInRange(5, 10)} {};
  ~Room(){};
  Room(const Room &) = delete;
  Room &operator=(const Room &) = delete;

  const GameDifficulty difficulty;

  void addPlayer(int playerId, std::string nickname);
  bool const isFull() const;
  void tick();

private:
  GameplayMessageBroker &messageBroker;

  enum GameplayState
  {
    WAITING_PLAYERS,
    STARTED,
    EXPECTING_INPUT,
    OVER,
  };

  const int NO_INPUT_TIMEOUT_S{10};
  const int ONE_PLAYER_ONLY_TIMEOUT_S{1};

  GameplayState gameplayState{WAITING_PLAYERS};

  std::array<std::optional<Player>, 2> players;
  int secondsBeforeInputRequest{};
  GameplayInput expectedInput{};
  std::optional<ChronoTimePoint> everyoneCanPlayAt{};
  std::optional<ChronoTimePoint> readyForInputAt{};

  void handleEveryoneReady();
  void handleEveryoneCanPlay();
  void handleTimeout();
  void handleOnePlayerPlayed();
  void handleEveryonePlayed();
  void handleReplay();

  void handlePlayerMessages(int playerId, std::shared_ptr<PlayerActionMessage>);
  void handlePlayerSetReadyStateMessage(Player *player, std::shared_ptr<SetReadyStatePlayerActionMessage> message);
  void handlePlayerNotifyCanPlayMessage(Player *player, std::shared_ptr<NotifyCanPlayPlayerActionMessage> message);
  void handlePlayerInputMessage(Player *player, std::shared_ptr<InputPlayerActionMessage> message);
  void handlePlayerSetReplayReadyStateMessage(Player *player, std::shared_ptr<SetReplayReadyStatePlayerActionMessage> message);
  void handlePlayerLeaveGameMessage(Player *player, std::shared_ptr<LeaveGamePlayerActionMessage> message);

  inline bool isEveryoneReady() const;
  inline bool everyoneCanPlay() const;
  inline bool everyonePlayed() const;
  inline bool noOnePlayed() const;
  inline bool everyoneWantsToReplay() const;

  std::optional<Player *> getPlayerByAssignment(PlayerAssignment assignment);
  std::optional<Player *> getPlayerById(int id);
  const bool isAssignmentUsed(const PlayerAssignment assignment) const;
  const std::optional<const PlayerAssignment> getAvailableAssignment() const;

  void broadcastGameplayMessage(std::shared_ptr<GameplayMessage> message) const;
  const GameplayInput getRandomGameplayInput() const;

  const GameResultDTO buildGameResultDTO(std::optional<PlayerAssignment> = std::nullopt) const;
};

#endif