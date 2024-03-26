#include "catch2/catch.hpp"
#include "gameplayMessageTestUtils.h"

#include "RoomManager.h"

SCENARIO("Room Manager", "[roomManager]") {
  MessageHub messageHub{};
  GameplayMessageBroker messageBroker{messageHub};
  GameplayMessageTestUtils utils{messageHub, messageBroker};

  RoomManager roomManager{messageBroker};

  GIVEN("A room manager with no room created yet") {
    WHEN("Adding a player") {
      int playerId = 1;

      roomManager.addPlayerToRoom(playerId, "playerNickname", MEDIUM);

      THEN("The player is notified a room was joined") {
        REQUIRE(utils.countMessagesPendingForPlayer(playerId) == 1);
        REQUIRE(utils.consumeOneGameplayMessageForPlayer(playerId)->messageType == ROOM_JOINED);
      }
    }
  }

  GIVEN("A room manager with a room already created and one player in it") {
    int firstPlayerId = 1;
    GameDifficulty difficulty = EASY;

    roomManager.addPlayerToRoom(firstPlayerId, "playerNickname", difficulty);
    utils.removeAllMessagesPendingForPlayer(firstPlayerId);

    WHEN("Adding a player") {
      int secondPlayerId = 2;
      AND_WHEN("The difficulty is the same") {
        roomManager.addPlayerToRoom(secondPlayerId, "playerNickname", difficulty);
        THEN("The player is added to the existing room") {
          REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
          REQUIRE(utils.consumeOneGameplayMessageForPlayer(firstPlayerId)->messageType == PLAYER_JOINED_GAME);

          REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);
          REQUIRE(utils.consumeOneGameplayMessageForPlayer(secondPlayerId)->messageType == ROOM_JOINED);
        }
      }

      AND_WHEN("The difficulty is different") {
        roomManager.addPlayerToRoom(secondPlayerId, "playerNickname", MEDIUM);
        THEN("The player is added to a different room") {
          REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 0);

          REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);
          REQUIRE(utils.consumeOneGameplayMessageForPlayer(secondPlayerId)->messageType == ROOM_JOINED);
        }
      }
    }
  }

  GIVEN("A room manager with a room already created and two players in it") {
    int firstPlayerId = 1;
    GameDifficulty difficulty = EASY;

    roomManager.addPlayerToRoom(firstPlayerId, "playerNickname", difficulty);

    int secondPlayerId = 2;
    roomManager.addPlayerToRoom(secondPlayerId, "playerNickname", difficulty);

    utils.removeAllMessagesPendingForPlayer(firstPlayerId);
    utils.removeAllMessagesPendingForPlayer(secondPlayerId);

    WHEN("Adding a player for the same difficulty") {
      int thirdPlayerId = 3;
      roomManager.addPlayerToRoom(thirdPlayerId, "playerNickname", difficulty);
      THEN("The player is added to a new room") {
        REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 0);
        REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 0);

        REQUIRE(utils.countMessagesPendingForPlayer(thirdPlayerId) == 1);
        REQUIRE(utils.consumeOneGameplayMessageForPlayer(thirdPlayerId)->messageType == ROOM_JOINED);
      }
    }
  }
}