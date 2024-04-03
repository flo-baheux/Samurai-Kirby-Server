#pragma once

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

class Room {
public:
  class RoomConfig {
  public:
    RoomConfig(
      GameDifficulty difficulty = EASY,
      int secondsBeforeInputRequest = getRandomValueInRange(5, 10),
      int noInputTimeout = 5,
      int onePlayerInputOnlyTimeout = 1
    )
      : difficulty{difficulty},
      secondsBeforeInputRequest{secondsBeforeInputRequest},
      noInputTimeout{noInputTimeout},
      onePlayerInputOnlyTimeout{onePlayerInputOnlyTimeout} {};

    const GameDifficulty difficulty;
    const int secondsBeforeInputRequest;
    const int noInputTimeout;
    const int onePlayerInputOnlyTimeout;
  };

  explicit Room(GameplayMessageBroker &messageBroker, RoomConfig config = {}) : config{config}, messageBroker{messageBroker} {};
  ~Room() {};
  Room(const Room &) = delete;
  Room &operator=(const Room &) = delete;

  void addPlayer(int playerId, std::string nickname);
  void removePlayer(int playerId);
  bool hasPlayerWithId(int playerId);
  bool isFull() const;
  bool isEmpty() const;
  void tick();

  RoomConfig config;

private:
  GameplayMessageBroker &messageBroker;

  enum GameplayState {
    WAITING_PLAYERS,
    STARTED,
    EXPECTING_INPUT,
    OVER,
  };

  GameplayState gameplayState{WAITING_PLAYERS};

  std::array<std::optional<Player>, 2> players;
  GameplayInput expectedInput{};
  std::optional<ChronoTimePoint> everyoneCanPlayAt{};
  std::optional<ChronoTimePoint> readyForInputAt{};

  void handleEveryoneReady();
  void handleEveryoneCanPlay();
  void handleTimeout();
  void handleOnePlayerPlayed();
  void handleEveryonePlayed();
  void handleReplay();

  void handlePlayerMessage(int playerId, std::shared_ptr<PlayerActionMessage>);
  void handlePlayerSetReadyStateMessage(Player *player, std::shared_ptr<SetReadyStatePlayerActionMessage> message);
  void handlePlayerNotifyCanPlayMessage(Player *player, std::shared_ptr<NotifyCanPlayPlayerActionMessage> message);
  void handlePlayerInputMessage(Player *player, std::shared_ptr<InputPlayerActionMessage> message);
  void handlePlayerSetReplayReadyStateMessage(Player *player, std::shared_ptr<SetReplayReadyStatePlayerActionMessage> message);
  void handlePlayerLeaveGameMessage(Player *player, std::shared_ptr<LeaveGamePlayerActionMessage> message);

  bool isEveryoneReady() const;
  bool everyoneCanPlay() const;
  bool everyonePlayed() const;
  bool noOnePlayed() const;
  bool everyoneWantsToReplay() const;

  std::optional<Player *> getPlayerByAssignment(PlayerAssignment assignment);
  std::optional<Player *> getPlayerById(int id);
  bool isAssignmentUsed(const PlayerAssignment assignment) const;
  std::optional<const PlayerAssignment> getAvailableAssignment() const;

  void broadcastGameplayMessage(std::shared_ptr<GameplayMessage> message) const;
  GameplayInput getRandomGameplayInput() const;

  GameResultDTO buildGameResultDTO(std::optional<PlayerAssignment> = std::nullopt) const;
};
