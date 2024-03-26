#include "catch2/catch.hpp"
#include "gameplayMessageTestUtils.h"

#include "Room.h"

SCENARIO("Rooms - Adding player", "[room][addingPlayer]") {
  MessageHub messageHub{};
  GameplayMessageBroker messageBroker{messageHub};
  GameplayMessageTestUtils utils{messageHub, messageBroker};

  GIVEN("An empty room") {
    Room room{messageBroker};

    REQUIRE(room.isEmpty());

    WHEN("A player is added to the room") {
      int playerId = 1;
      room.addPlayer(playerId, "P1");

      REQUIRE(room.hasPlayerWithId(playerId));
      THEN("The room is not empty anymore") {
        REQUIRE(!room.isEmpty());
      }

      THEN("A message is published to notify this player he joined a game") {
        REQUIRE(utils.countMessagesPendingForPlayer(playerId) == 1);
        auto message = utils.consumeOneGameplayMessageForPlayer(playerId);

        REQUIRE(message->messageType == ROOM_JOINED);
        auto roomJoinedMessage = utils.unwrapGameplayMessage<RoomJoinedMessage>(message);

        REQUIRE(roomJoinedMessage.playerAssignment == P1);
        REQUIRE(roomJoinedMessage.otherPlayer.has_value() == false);
      }
    }
  }

  GIVEN("A room with already one player in it") {
    Room room{messageBroker, MEDIUM};
    int firstPlayerId = 1;
    std::string firstPlayerNickname = "P1";
    room.addPlayer(firstPlayerId, firstPlayerNickname);

    REQUIRE(room.hasPlayerWithId(firstPlayerId));
    utils.removeAllMessagesPendingForPlayer(firstPlayerId);

    WHEN("A second player is added to the room") {
      int secondPlayerId = 2;
      std::string secondPlayerNickname = "P2";
      room.addPlayer(secondPlayerId, secondPlayerNickname);

      REQUIRE(room.hasPlayerWithId(secondPlayerId));

      THEN("The room is full") {
        REQUIRE(room.isFull());
      }

      THEN("A message is published to notify this player he joined a game") {
        REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);
        auto message = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

        REQUIRE(message->messageType == ROOM_JOINED);
        auto roomJoinedMessage = utils.unwrapGameplayMessage<RoomJoinedMessage>(message);

        REQUIRE(roomJoinedMessage.playerAssignment == P2);
        REQUIRE(roomJoinedMessage.otherPlayer.has_value() == true);
        REQUIRE(roomJoinedMessage.otherPlayer.value().assignment == P1);
        REQUIRE(roomJoinedMessage.otherPlayer.value().nickname == firstPlayerNickname);
        REQUIRE(roomJoinedMessage.otherPlayer.value().isReady == false);
      }

      THEN("Another message is published to the first player to notify someone joined his game") {
        REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
        auto message = utils.consumeOneGameplayMessageForPlayer(firstPlayerId);

        REQUIRE(message->messageType == PLAYER_JOINED_GAME);
        auto playerJoinedRoomMessage = utils.unwrapGameplayMessage<PlayerJoinedRoomMessage>(message);

        REQUIRE(playerJoinedRoomMessage.player.assignment == P2);
        REQUIRE(playerJoinedRoomMessage.player.nickname == secondPlayerNickname);
        REQUIRE(playerJoinedRoomMessage.player.isReady == false);
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

    WHEN("A third player is added to the room") {
      int thirdPlayerId = 3;
      std::string thirdPlayerNickname = "P3???";

      room.addPlayer(thirdPlayerId, thirdPlayerNickname);

      THEN("The player is not added") {
        REQUIRE(!room.hasPlayerWithId(thirdPlayerId));
      }

      THEN("No messages are sent to players in the room") {
        REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 0);
        REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 0);
      }
    }
  }
}

//   std::cout << "scenario inside called" << std::endl;

//   GIVEN("A player with ID 1") {
//     int playerId = 1;
//     messageBroker.subscribeToPlayerActionMessages(playerId,
//       [&](int playerId, std::shared_ptr<PlayerActionMessage> e) {
//         roomManager.handlePlayerMessage(playerId, e);
//       });

//     WHEN("The player requests to join a room on MEDIUM difficulty") {
//       auto msg = std::make_shared<JoinGamePlayerActionMessage>(
//         "Nickname",
//         MEDIUM
//       );

//       messageHub.playerActionMessagesByPlayerId[playerId].push(msg);
//       messageBroker.dispatchMessages();
//       roomManager.tickAllRooms();

//       THEN("A message is sent to the player notifying him he joined a room") {
//         REQUIRE(messageHub.gameplayMessagesByPlayerId[playerId].size() == 1);

//         std::shared_ptr<GameplayMessage> message = messageHub.gameplayMessagesByPlayerId[playerId].front();
//         messageHub.gameplayMessagesByPlayerId[playerId].pop();

//         REQUIRE(message->messageType == PLAYER_JOINED_GAME);
//       }
//       THEN("The room is waiting for another player") {}
//     }
//   }

// }