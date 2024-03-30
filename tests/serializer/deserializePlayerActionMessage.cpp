#include "catch2/catch.hpp"
#include "Serializer.h"

SCENARIO("Deserializing Player Action Messages", "[Serialize][PlayerActionMessage]") {

  GIVEN("A JoinGamePlayerActionMessage") {
    const std::string message = R"({"messageType":0,"nickname":"playerNickname","difficulty":0})";

    THEN("It correctly deserializes into a PlayerActionMessage") {
      const std::shared_ptr<PlayerActionMessage> playerActionMessage = Serializer::deserializePlayerActionMessage(message);

      REQUIRE(playerActionMessage->messageType == JOIN_GAME);
      JoinGamePlayerActionMessage joinGamePlayerActionMessage
        = dynamic_cast<JoinGamePlayerActionMessage &>(*playerActionMessage);
      REQUIRE(joinGamePlayerActionMessage.nickname == "playerNickname");
      REQUIRE(joinGamePlayerActionMessage.gameDifficulty == EASY);
    }
  }

  GIVEN("A SetReadyStatePlayerActionMessage") {
    const std::string message = R"({"messageType":1,"isReady":true})";

    THEN("It correctly deserializes into a SetReadyStatePlayerActionMessage") {
      const std::shared_ptr<PlayerActionMessage> playerActionMessage = Serializer::deserializePlayerActionMessage(message);

      REQUIRE(playerActionMessage->messageType == SET_READY_STATE);
      SetReadyStatePlayerActionMessage setReadyStatePlayerActionMessage
        = dynamic_cast<SetReadyStatePlayerActionMessage &>(*playerActionMessage);
      REQUIRE(setReadyStatePlayerActionMessage.isReady == true);
    }
  }

  GIVEN("A NotifyCanPlayPlayerActionMessage") {
    const std::string message = R"({"messageType":2})";

    THEN("It correctly deserializes into a NotifyCanPlayPlayerActionMessage") {
      const std::shared_ptr<PlayerActionMessage> playerActionMessage = Serializer::deserializePlayerActionMessage(message);

      REQUIRE(playerActionMessage->messageType == NOTIFY_CAN_PLAY);
      NotifyCanPlayPlayerActionMessage notifyCanPlayPlayerActionMessage
        = dynamic_cast<NotifyCanPlayPlayerActionMessage &>(*playerActionMessage);
      REQUIRE(notifyCanPlayPlayerActionMessage.messageType == NOTIFY_CAN_PLAY);
    }
  }

  GIVEN("A InputPlayerActionMessage") {
    const std::string message = R"({"messageType":3,"inputPressed":0,"timeToPress":200})";

    THEN("It correctly deserializes into a InputPlayerActionMessage") {
      const std::shared_ptr<PlayerActionMessage> playerActionMessage = Serializer::deserializePlayerActionMessage(message);

      REQUIRE(playerActionMessage->messageType == PLAYER_INPUT);
      InputPlayerActionMessage inputPlayerActionMessage
        = dynamic_cast<InputPlayerActionMessage &>(*playerActionMessage);
      REQUIRE(inputPlayerActionMessage.inputPressed == NORTH);
      REQUIRE(inputPlayerActionMessage.timeToPress == 200);
    }
  }

  GIVEN("A SetReplayReadyStatePlayerActionMessage") {
    const std::string message = R"({"messageType":4,"isReadyToReplay":true})";

    THEN("It correctly deserializes into a SetReplayReadyStatePlayerActionMessage") {
      const std::shared_ptr<PlayerActionMessage> playerActionMessage = Serializer::deserializePlayerActionMessage(message);

      REQUIRE(playerActionMessage->messageType == SET_REPLAY_READY_STATE);
      SetReplayReadyStatePlayerActionMessage setReplayReadyStatePlayerActionMessage
        = dynamic_cast<SetReplayReadyStatePlayerActionMessage &>(*playerActionMessage);
      REQUIRE(setReplayReadyStatePlayerActionMessage.isReadyToReplay == true);
    }
  }

  GIVEN("A LeaveGamePlayerActionMessage") {
    const std::string message = R"({"messageType":5})";

    THEN("It correctly deserializes into a LeaveGamePlayerActionMessage") {
      const std::shared_ptr<PlayerActionMessage> playerActionMessage = Serializer::deserializePlayerActionMessage(message);

      REQUIRE(playerActionMessage->messageType == LEAVE_GAME);
      LeaveGamePlayerActionMessage leaveGamePlayerActionMessage
        = dynamic_cast<LeaveGamePlayerActionMessage &>(*playerActionMessage);
      REQUIRE(leaveGamePlayerActionMessage.messageType == LEAVE_GAME);
    }
  }
}