#include "catch2/catch.hpp"
#include "Serializer.h"

SCENARIO("Serializing Gameplay Messages", "[Serialize][GameplayMessage]") {
  PlayerLobbyDTO p1Lobby{"Player1", P1, false};
  PlayerLobbyDTO p2Lobby{"Player2", P2, true};

  PlayerGameResultDTO p1GameResultWithInput{P1, SOUTH, 250};
  PlayerGameResultDTO p1GameResultNoInput{P1};
  PlayerGameResultDTO p2GameResultWithInput{P2, SOUTH, 300};
  PlayerGameResultDTO p2GameResultNoInput{P2};

  GIVEN("A RoomJoinedMessage") {
    WHEN("There is no other player in the message") {
      RoomJoinedMessage message{P1};

      THEN("The result is properly stringified") {
        REQUIRE(
          Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
          ==
          R"({"messageType":0,"otherPlayer":null,"playerAssignment":0})"
        );
      }
    }

    WHEN("There is another player in the message") {
      RoomJoinedMessage message{P1, p2Lobby};

      THEN("The result is properly stringified") {
        REQUIRE(
          Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
          ==
          R"({"messageType":0,"otherPlayer":{"assignment":1,"isReady":true,"nickname":"Player2"},"playerAssignment":0})"
        );
      }
    }
  }

  GIVEN("A PlayerJoinedRoomMessage") {
    PlayerJoinedRoomMessage message{p2Lobby};

    THEN("The result is properly stringified") {
      REQUIRE(
        Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
        ==
        R"({"messageType":1,"player":{"assignment":1,"isReady":true,"nickname":"Player2"}})"
      );
    }
  }

  GIVEN("A PlayerLeftRoomMessage") {
    PlayerLeftRoomMessage message{P2};

    THEN("The result is properly stringified") {
      REQUIRE(
        Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
        ==
        R"({"messageType":2,"playerAssignment":1})"
      );
    }
  }

  GIVEN("A PlayerReadyStateChangedMessage") {
    PlayerReadyStateChangedMessage message{P2, true};

    THEN("The result is properly stringified") {
      REQUIRE(
        Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
        ==
        R"({"isReady":true,"messageType":3,"playerAssignment":1})"
      );
    }
  }

  GIVEN("A AllReadyGameStartingMessage") {
    AllReadyGameStartingMessage message{};

    THEN("The result is properly stringified") {
      REQUIRE(
        Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
        ==
        R"({"messageType":4})"
      );
    }
  }

  GIVEN("A GameExpectingInputMessage") {
    GameExpectingInputMessage message{SOUTH};

    THEN("The result is properly stringified") {
      REQUIRE(
        Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
        ==
        R"({"expectedInput":3,"messageType":5})"
      );
    }
  }

  GIVEN("A GameOverEarlyInputMessage") {
    WHEN("Only one player played") {
      GameOverEarlyInputMessage message{
        {
          P1,
          p1GameResultWithInput,
          p2GameResultNoInput,
        },
      };

      THEN("The result is properly stringified") {
        REQUIRE(
          Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
          ==
          R"({"gameResult":{"player1":{"assignment":0,"inputPressed":3,"timeToPress":250},"player2":{"assignment":1,"inputPressed":null,"timeToPress":null},"winningPlayer":0},"messageType":6})"
        );
      }
    }

    WHEN("Two players played") {
      GameOverEarlyInputMessage message{
        {
          P1,
          p1GameResultWithInput,
          p2GameResultWithInput,
        },
      };

      THEN("The result is properly stringified") {
        REQUIRE(
          Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
          ==
          R"({"gameResult":{"player1":{"assignment":0,"inputPressed":3,"timeToPress":250},"player2":{"assignment":1,"inputPressed":3,"timeToPress":300},"winningPlayer":0},"messageType":6})"
        );
      }
    }
  }

  GIVEN("A GameOverWrongInputMessage") {
    WHEN("Only one player played") {
      GameOverWrongInputMessage message{
        {
          P1,
          p1GameResultWithInput,
          p2GameResultNoInput,
        },
      };

      THEN("The result is properly stringified") {
        REQUIRE(
          Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
          ==
          R"({"gameResult":{"player1":{"assignment":0,"inputPressed":3,"timeToPress":250},"player2":{"assignment":1,"inputPressed":null,"timeToPress":null},"winningPlayer":0},"messageType":7})"
        );
      }
    }

    WHEN("Two players played") {
      GameOverWrongInputMessage message{
        {
          P1,
          p1GameResultWithInput,
          p2GameResultWithInput,
        },
      };

      THEN("The result is properly stringified") {
        REQUIRE(
          Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
          ==
          R"({"gameResult":{"player1":{"assignment":0,"inputPressed":3,"timeToPress":250},"player2":{"assignment":1,"inputPressed":3,"timeToPress":300},"winningPlayer":0},"messageType":7})"
        );
      }
    }
  }

  GIVEN("A GameOverWinMessage") {
    WHEN("Only one player played") {
      GameOverWinMessage message{
        {
          P1,
          p1GameResultWithInput,
          p2GameResultNoInput,
        },
      };

      THEN("The result is properly stringified") {
        REQUIRE(
          Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
          ==
          R"({"gameResult":{"player1":{"assignment":0,"inputPressed":3,"timeToPress":250},"player2":{"assignment":1,"inputPressed":null,"timeToPress":null},"winningPlayer":0},"messageType":8})"
        );
      }
    }

    WHEN("Two players played") {
      GameOverWinMessage message{
        {
          P1,
          p1GameResultWithInput,
          p2GameResultWithInput,
        },
      };

      THEN("The result is properly stringified") {
        REQUIRE(
          Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
          ==
          R"({"gameResult":{"player1":{"assignment":0,"inputPressed":3,"timeToPress":250},"player2":{"assignment":1,"inputPressed":3,"timeToPress":300},"winningPlayer":0},"messageType":8})"
        );
      }
    }
  }

  GIVEN("A GameOverDrawMessage") {
    WHEN("No player played") {
      GameOverDrawMessage message{
        {
          P1,
          p1GameResultNoInput,
          p2GameResultNoInput,
        },
      };

      THEN("The result is properly stringified") {
        REQUIRE(
          Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
          ==
          R"({"gameResult":{"player1":{"assignment":0,"inputPressed":null,"timeToPress":null},"player2":{"assignment":1,"inputPressed":null,"timeToPress":null},"winningPlayer":0},"messageType":9})"
        );
      }
    }

    WHEN("Two players played") {
      GameOverDrawMessage message{
        {
          P1,
          p1GameResultWithInput,
          p2GameResultWithInput,
        },
      };

      THEN("The result is properly stringified") {
        REQUIRE(
          Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
          ==
          R"({"gameResult":{"player1":{"assignment":0,"inputPressed":3,"timeToPress":250},"player2":{"assignment":1,"inputPressed":3,"timeToPress":300},"winningPlayer":0},"messageType":9})"
        );
      }
    }
  }

  GIVEN("A PlayerReplayReadyStateChangedMessage") {
    PlayerReplayReadyStateChangedMessage message{P1, true};

    THEN("The result is properly stringified") {
      REQUIRE(
        Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
        ==
        R"({"gameResult":{"player1":{"assignment":0,"inputPressed":3,"timeToPress":100},"player2":{"assignment":1,"inputPressed":null,"timeToPress":null},"winningPlayer":0},"messageType":6})"
      );
    }
  }

  GIVEN("A AllReplayReadyGameStartingMessage") {
    AllReplayReadyGameStartingMessage message{};

    THEN("The result is properly stringified") {
      REQUIRE(
        Serializer::serializeGameplayMessage(dynamic_cast<GameplayMessage &>(message))
        ==
        R"({"gameResult":{"player1":{"assignment":0,"inputPressed":3,"timeToPress":100},"player2":{"assignment":1,"inputPressed":null,"timeToPress":null},"winningPlayer":0},"messageType":6})"
      );
    }
  }
}