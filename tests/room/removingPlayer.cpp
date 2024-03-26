#include "catch2/catch.hpp"
#include "gameplayMessageTestUtils.h"

#include "Room.h"

SCENARIO("Rooms - Removing player", "[room][removingPlayer]") {
  MessageHub messageHub{};
  GameplayMessageBroker messageBroker{messageHub};
  GameplayMessageTestUtils utils{messageHub, messageBroker};

  GIVEN("An empty room") {
    Room room{messageBroker, MEDIUM};

    REQUIRE(room.isEmpty());

    WHEN("An attempt is made to remove a player from the room") {
      int playerId = 1;

      REQUIRE(!room.hasPlayerWithId(playerId));
      room.removePlayer(playerId);

      THEN("No message is published for this player") {
        REQUIRE(utils.countMessagesPendingForPlayer(playerId) == 0);
      }
    }
  }

  GIVEN("A room with already one player in it") {
    Room room{messageBroker, MEDIUM};
    int playerId = 1;
    std::string firstPlayerNickname = "P1";
    room.addPlayer(playerId, firstPlayerNickname);
    utils.removeAllMessagesPendingForPlayer(playerId);

    REQUIRE(room.hasPlayerWithId(playerId));

    WHEN("The player is removed from the room") {
      room.removePlayer(playerId);

      THEN("The player is not in the room anymore") {
        REQUIRE(!room.hasPlayerWithId(playerId));
      }

      THEN("The room is empty") {
        REQUIRE(room.isEmpty());
      }

      THEN("No message is published for this player") {
        REQUIRE(utils.countMessagesPendingForPlayer(playerId) == 0);
      }
    }
  }

  GIVEN("A full room") {
    Room room{messageBroker, MEDIUM};

    int firstPlayerId = 1;
    std::string firstPlayerNickname = "P1";
    room.addPlayer(firstPlayerId, firstPlayerNickname);
    REQUIRE(room.hasPlayerWithId(firstPlayerId) == true);

    int secondPlayerId = 2;
    std::string secondPlayerNickname = "P2";
    room.addPlayer(secondPlayerId, secondPlayerNickname);
    REQUIRE(room.hasPlayerWithId(secondPlayerId) == true);

    utils.removeAllMessagesPendingForPlayer(firstPlayerId);
    utils.removeAllMessagesPendingForPlayer(secondPlayerId);

    REQUIRE(room.isFull());

    WHEN("The first player is removed from the room") {
      room.removePlayer(firstPlayerId);

      THEN("The player is removed") {
        REQUIRE(!room.hasPlayerWithId(firstPlayerId));
      }
      THEN("No message is published for this player") {
        REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 0);
      }

      THEN("The second player is notified") {
        REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);
        auto message = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

        REQUIRE(message->messageType == PLAYER_LEFT_GAME);
        auto playerLeftRoomMessage = utils.unwrapGameplayMessage<PlayerLeftRoomMessage>(message);

        REQUIRE(playerLeftRoomMessage.playerAssignment == P1);
      }
    }
  }
}