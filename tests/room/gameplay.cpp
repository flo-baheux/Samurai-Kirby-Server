#include "catch2/catch.hpp"
#include "gameplayMessageTestUtils.h"

#include "Room.h"

SCENARIO("Rooms - Waiting for players", "[room][gameplay]") {
  MessageHub messageHub{};
  GameplayMessageBroker messageBroker{messageHub};
  GameplayMessageTestUtils utils{messageHub, messageBroker};

  GIVEN("A room with two players in it, easy, no timeout") {
    Room room{messageBroker, Room::RoomConfig{EASY, 0, 0, 0}};

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

    AND_GIVEN("The game has not started yet") {
      WHEN("A player changes his ready state") {
        auto playerActionMessage = std::make_shared<SetReadyStatePlayerActionMessage>(true);
        utils.publishDispatchActionMessageFromPlayer(firstPlayerId, playerActionMessage);
        room.tick();

        THEN("Both players are notified with the new player state") {
          REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
          REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);

          auto firstPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(firstPlayerId);
          auto secondPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

          REQUIRE(firstPlayerGameplayMessage->messageType == PLAYER_READY_STATE_CHANGED);
          REQUIRE(secondPlayerGameplayMessage->messageType == PLAYER_READY_STATE_CHANGED);

          auto firstPlayerReadyStateChangedMessage = utils.unwrapGameplayMessage<PlayerReadyStateChangedMessage>(firstPlayerGameplayMessage);
          auto secondPlayerReadyStateChangedMessage = utils.unwrapGameplayMessage<PlayerReadyStateChangedMessage>(secondPlayerGameplayMessage);

          REQUIRE(firstPlayerReadyStateChangedMessage.playerAssignment == P1);
          REQUIRE(secondPlayerReadyStateChangedMessage.playerAssignment == P1);

          REQUIRE(firstPlayerReadyStateChangedMessage.isReady == true);
          REQUIRE(secondPlayerReadyStateChangedMessage.isReady == true);

          AND_WHEN("Both players are ready") {
            auto playerActionMessage = std::make_shared<SetReadyStatePlayerActionMessage>(true);
            utils.publishDispatchActionMessageFromPlayer(secondPlayerId, playerActionMessage);

            utils.removeAllMessagesPendingForPlayer(firstPlayerId);
            utils.removeAllMessagesPendingForPlayer(secondPlayerId);

            room.tick();

            THEN("Both players are notified everyone is ready and the game will start") {
              REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
              REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);

              auto firstPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(firstPlayerId);
              auto secondPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

              REQUIRE(firstPlayerGameplayMessage->messageType == ALL_READY_GAME_STARTING);
              REQUIRE(secondPlayerGameplayMessage->messageType == ALL_READY_GAME_STARTING);
            }
          }
        }
      }
    }
  }
}

SCENARIO("Rooms - Started but not expecting input yet", "[room][gameplay]") {
  MessageHub messageHub{};
  GameplayMessageBroker messageBroker{messageHub};
  GameplayMessageTestUtils utils{messageHub, messageBroker};

  GIVEN("A room with two players in it, easy, no timeout") {
    Room room{messageBroker, Room::RoomConfig{EASY, 0, 0, 0}};

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

    AND_GIVEN("The game is starting") {
      auto setReadyStatePlayerActionMessage = std::make_shared<SetReadyStatePlayerActionMessage>(true);
      utils.publishDispatchActionMessageFromPlayer(firstPlayerId, setReadyStatePlayerActionMessage);
      utils.publishDispatchActionMessageFromPlayer(secondPlayerId, setReadyStatePlayerActionMessage);

      utils.removeAllMessagesPendingForPlayer(firstPlayerId);
      utils.removeAllMessagesPendingForPlayer(secondPlayerId);

      room.tick();

      REQUIRE(utils.consumeOneGameplayMessageForPlayer(firstPlayerId)->messageType == ALL_READY_GAME_STARTING);
      REQUIRE(utils.consumeOneGameplayMessageForPlayer(secondPlayerId)->messageType == ALL_READY_GAME_STARTING);

      WHEN("Both players notify they are ready to play") {
        auto notifyCanPlayPlayerActionMessage = std::make_shared<NotifyCanPlayPlayerActionMessage>();
        utils.publishDispatchActionMessageFromPlayer(firstPlayerId, notifyCanPlayPlayerActionMessage);
        utils.publishDispatchActionMessageFromPlayer(secondPlayerId, notifyCanPlayPlayerActionMessage);

        room.tick();
        THEN("Both players are notified the game is now expecting an input") {
          REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
          REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);

          auto firstPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(firstPlayerId);
          auto secondPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

          REQUIRE(firstPlayerGameplayMessage->messageType == GAME_EXPECTING_INPUT);
          REQUIRE(secondPlayerGameplayMessage->messageType == GAME_EXPECTING_INPUT);

          auto firstPlayerGameExpectingInput = utils.unwrapGameplayMessage<GameExpectingInputMessage>(firstPlayerGameplayMessage);
          auto secondPlayerGameExpectingInput = utils.unwrapGameplayMessage<GameExpectingInputMessage>(secondPlayerGameplayMessage);

          REQUIRE(firstPlayerGameExpectingInput.expectedInput == GameplayInput::SOUTH);
          REQUIRE(secondPlayerGameExpectingInput.expectedInput == GameplayInput::SOUTH);
        }
      }
    }
  }
}

SCENARIO("Rooms - Expecting input to game over", "[room][gameplay]") {
  MessageHub messageHub{};
  GameplayMessageBroker messageBroker{messageHub};
  GameplayMessageTestUtils utils{messageHub, messageBroker};

  GIVEN("A room with two players in it, easy, no timeout") {
    Room room{messageBroker, Room::RoomConfig{EASY, 0, 0, 0}};

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

    AND_GIVEN("The game started and is expecting an input") {
      auto setReadyStatePlayerActionMessage = std::make_shared<SetReadyStatePlayerActionMessage>(true);
      utils.publishDispatchActionMessageFromPlayer(firstPlayerId, setReadyStatePlayerActionMessage);
      utils.publishDispatchActionMessageFromPlayer(secondPlayerId, setReadyStatePlayerActionMessage);
      room.tick();

      auto notifyCanPlayPlayerActionMessage = std::make_shared<NotifyCanPlayPlayerActionMessage>();
      utils.publishDispatchActionMessageFromPlayer(firstPlayerId, notifyCanPlayPlayerActionMessage);
      utils.publishDispatchActionMessageFromPlayer(secondPlayerId, notifyCanPlayPlayerActionMessage);
      room.tick();

      utils.removeAllMessagesPendingForPlayer(firstPlayerId);
      utils.removeAllMessagesPendingForPlayer(secondPlayerId);

      WHEN("No player input received (testing with no timeout means we need one tick)") {
        room.tick();

        THEN("Both players are notified with the game result - draw") {
          REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
          REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);

          auto firstPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(firstPlayerId);
          auto secondPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

          REQUIRE(firstPlayerGameplayMessage->messageType == GAME_OVER_DRAW);
          REQUIRE(secondPlayerGameplayMessage->messageType == GAME_OVER_DRAW);

          auto firstPlayerGameOverDrawMessage = utils.unwrapGameplayMessage<GameOverDrawMessage>(firstPlayerGameplayMessage);
          auto secondPlayerGameOverDrawMessage = utils.unwrapGameplayMessage<GameOverDrawMessage>(secondPlayerGameplayMessage);

          REQUIRE(firstPlayerGameOverDrawMessage.gameResult.winningPlayer.has_value() == false);
          REQUIRE(firstPlayerGameOverDrawMessage.gameResult.player1.assignment == P1);
          REQUIRE(firstPlayerGameOverDrawMessage.gameResult.player1.inputPressed.has_value() == false);
          REQUIRE(firstPlayerGameOverDrawMessage.gameResult.player1.timeToPress.has_value() == false);
          REQUIRE(firstPlayerGameOverDrawMessage.gameResult.player2.assignment == P2);
          REQUIRE(firstPlayerGameOverDrawMessage.gameResult.player2.inputPressed.has_value() == false);
          REQUIRE(firstPlayerGameOverDrawMessage.gameResult.player2.timeToPress.has_value() == false);


          REQUIRE(secondPlayerGameOverDrawMessage.gameResult.winningPlayer.has_value() == false);
          REQUIRE(secondPlayerGameOverDrawMessage.gameResult.player1.assignment == P1);
          REQUIRE(secondPlayerGameOverDrawMessage.gameResult.player1.inputPressed.has_value() == false);
          REQUIRE(secondPlayerGameOverDrawMessage.gameResult.player1.timeToPress.has_value() == false);
          REQUIRE(secondPlayerGameOverDrawMessage.gameResult.player2.assignment == P2);
          REQUIRE(secondPlayerGameOverDrawMessage.gameResult.player2.inputPressed.has_value() == false);
          REQUIRE(secondPlayerGameOverDrawMessage.gameResult.player2.timeToPress.has_value() == false);
        }
      }

      WHEN("Only one player is providing an input") {
        GameplayInput expectedInput = SOUTH;

        AND_WHEN("The input is the expected one") {
          int timeToPress = 200;
          auto inputPlayerActionMessage = std::make_shared<InputPlayerActionMessage>(expectedInput, timeToPress);

          utils.publishDispatchActionMessageFromPlayer(firstPlayerId, inputPlayerActionMessage);

          room.tick();

          THEN("Both players are notified with the game result - the player who played won") {
            REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
            REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);

            auto firstPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(firstPlayerId);
            auto secondPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

            REQUIRE(firstPlayerGameplayMessage->messageType == GAME_OVER_WIN);
            REQUIRE(secondPlayerGameplayMessage->messageType == GAME_OVER_WIN);

            auto firstPlayerGameOverWinMessage = utils.unwrapGameplayMessage<GameOverWinMessage>(firstPlayerGameplayMessage);
            auto secondPlayerGameOverWinMessage = utils.unwrapGameplayMessage<GameOverWinMessage>(secondPlayerGameplayMessage);

            REQUIRE(firstPlayerGameOverWinMessage.gameResult.winningPlayer == P1);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.assignment == P1);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.inputPressed.value() == expectedInput);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.timeToPress.value() == timeToPress);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.assignment == P2);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.inputPressed.has_value() == false);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.timeToPress.has_value() == false);


            REQUIRE(secondPlayerGameOverWinMessage.gameResult.winningPlayer == P1);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.assignment == P1);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.inputPressed.value() == expectedInput);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.timeToPress.value() == timeToPress);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.assignment == P2);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.inputPressed.has_value() == false);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.timeToPress.has_value() == false);
          }
        }

        AND_WHEN("The input is wrong") {
          int timeToPress = 200;
          GameplayInput firstPlayerInput = EAST;
          auto inputPlayerActionMessage = std::make_shared<InputPlayerActionMessage>(firstPlayerInput, timeToPress);

          utils.publishDispatchActionMessageFromPlayer(firstPlayerId, inputPlayerActionMessage);
          room.tick();
          THEN("Both players are notified with the game result - the player who did not play won") {
            REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
            REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);

            auto firstPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(firstPlayerId);
            auto secondPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

            REQUIRE(firstPlayerGameplayMessage->messageType == GAME_OVER_WRONG_INPUT);
            REQUIRE(secondPlayerGameplayMessage->messageType == GAME_OVER_WRONG_INPUT);

            auto firstPlayerGameOverWrongInputMessage = utils.unwrapGameplayMessage<GameOverWrongInputMessage>(firstPlayerGameplayMessage);
            auto secondPlayerGameOverWrongInputMessage = utils.unwrapGameplayMessage<GameOverWrongInputMessage>(secondPlayerGameplayMessage);

            REQUIRE(firstPlayerGameOverWrongInputMessage.gameResult.winningPlayer == P2);
            REQUIRE(firstPlayerGameOverWrongInputMessage.gameResult.player1.assignment == P1);
            REQUIRE(firstPlayerGameOverWrongInputMessage.gameResult.player1.inputPressed.value() == firstPlayerInput);
            REQUIRE(firstPlayerGameOverWrongInputMessage.gameResult.player1.timeToPress.value() == timeToPress);
            REQUIRE(firstPlayerGameOverWrongInputMessage.gameResult.player2.assignment == P2);
            REQUIRE(firstPlayerGameOverWrongInputMessage.gameResult.player2.inputPressed.has_value() == false);
            REQUIRE(firstPlayerGameOverWrongInputMessage.gameResult.player2.timeToPress.has_value() == false);


            REQUIRE(secondPlayerGameOverWrongInputMessage.gameResult.winningPlayer == P2);
            REQUIRE(secondPlayerGameOverWrongInputMessage.gameResult.player1.assignment == P1);
            REQUIRE(secondPlayerGameOverWrongInputMessage.gameResult.player1.inputPressed.value() == firstPlayerInput);
            REQUIRE(secondPlayerGameOverWrongInputMessage.gameResult.player1.timeToPress.value() == timeToPress);
            REQUIRE(secondPlayerGameOverWrongInputMessage.gameResult.player2.assignment == P2);
            REQUIRE(secondPlayerGameOverWrongInputMessage.gameResult.player2.inputPressed.has_value() == false);
            REQUIRE(secondPlayerGameOverWrongInputMessage.gameResult.player2.timeToPress.has_value() == false);
          }
        }
      }

      WHEN("Both players are providing an input (testing with no timeout means we need one tick)") {
        GameplayInput expectedInput = SOUTH;

        AND_WHEN("Both input are correct") {
          GameplayInput firstPlayerInput = expectedInput;
          int firstPlayerTimeToPress = 200;
          GameplayInput secondPlayerInput = expectedInput;
          int secondPlayerTimeToPress = 300;
          auto firstPlayerInputPlayerActionMessage = std::make_shared<InputPlayerActionMessage>(firstPlayerInput, firstPlayerTimeToPress);
          auto secondPlayerInputPlayerActionMessage = std::make_shared<InputPlayerActionMessage>(secondPlayerInput, secondPlayerTimeToPress);

          utils.publishDispatchActionMessageFromPlayer(firstPlayerId, firstPlayerInputPlayerActionMessage);
          utils.publishDispatchActionMessageFromPlayer(secondPlayerId, secondPlayerInputPlayerActionMessage);
          room.tick();

          THEN("Both players are notified with the game result - the fastest player won") {
            REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
            REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);

            auto firstPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(firstPlayerId);
            auto secondPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

            REQUIRE(firstPlayerGameplayMessage->messageType == GAME_OVER_WIN);
            REQUIRE(secondPlayerGameplayMessage->messageType == GAME_OVER_WIN);

            auto firstPlayerGameOverWinMessage = utils.unwrapGameplayMessage<GameOverWinMessage>(firstPlayerGameplayMessage);
            auto secondPlayerGameOverWinMessage = utils.unwrapGameplayMessage<GameOverWinMessage>(secondPlayerGameplayMessage);

            REQUIRE(firstPlayerGameOverWinMessage.gameResult.winningPlayer == P1);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.assignment == P1);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.inputPressed == firstPlayerInput);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.timeToPress == firstPlayerTimeToPress);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.assignment == P2);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.inputPressed == secondPlayerInput);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.timeToPress == secondPlayerTimeToPress);


            REQUIRE(secondPlayerGameOverWinMessage.gameResult.winningPlayer == P1);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.assignment == P1);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.inputPressed == firstPlayerInput);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.timeToPress == firstPlayerTimeToPress);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.assignment == P2);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.inputPressed == secondPlayerInput);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.timeToPress == secondPlayerTimeToPress);
          }
        }

        AND_WHEN("One input is wrong") {
          int timeToPress = 200;
          GameplayInput firstPlayerInput = expectedInput;
          GameplayInput secondPlayerInput = EAST;
          auto firstPlayerInputPlayerActionMessage = std::make_shared<InputPlayerActionMessage>(firstPlayerInput, timeToPress);
          auto secondPlayerInputPlayerActionMessage = std::make_shared<InputPlayerActionMessage>(secondPlayerInput, timeToPress);

          utils.publishDispatchActionMessageFromPlayer(firstPlayerId, firstPlayerInputPlayerActionMessage);
          utils.publishDispatchActionMessageFromPlayer(secondPlayerId, secondPlayerInputPlayerActionMessage);
          room.tick();
          THEN("Both players are notified with the game result - the player with correct input won") {
            REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
            REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);

            auto firstPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(firstPlayerId);
            auto secondPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

            REQUIRE(firstPlayerGameplayMessage->messageType == GAME_OVER_WIN);
            REQUIRE(secondPlayerGameplayMessage->messageType == GAME_OVER_WIN);

            auto firstPlayerGameOverWinMessage = utils.unwrapGameplayMessage<GameOverWinMessage>(firstPlayerGameplayMessage);
            auto secondPlayerGameOverWinMessage = utils.unwrapGameplayMessage<GameOverWinMessage>(secondPlayerGameplayMessage);

            REQUIRE(firstPlayerGameOverWinMessage.gameResult.winningPlayer == P1);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.assignment == P1);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.inputPressed == firstPlayerInput);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.timeToPress == timeToPress);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.assignment == P2);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.inputPressed == secondPlayerInput);
            REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.timeToPress == timeToPress);

            REQUIRE(secondPlayerGameOverWinMessage.gameResult.winningPlayer == P1);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.assignment == P1);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.inputPressed == firstPlayerInput);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.timeToPress == timeToPress);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.assignment == P2);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.inputPressed == secondPlayerInput);
            REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.timeToPress == timeToPress);
          }
        }

        GameplayInput firstPlayerInput = SOUTH;
        int timeToPress = 200;
        auto inputPlayerActionMessage = std::make_shared<InputPlayerActionMessage>(expectedInput, timeToPress);
        utils.publishDispatchActionMessageFromPlayer(firstPlayerId, inputPlayerActionMessage);
        room.tick();
        THEN("Both players are notified with the game result - the player who played won") {
          REQUIRE(utils.countMessagesPendingForPlayer(firstPlayerId) == 1);
          REQUIRE(utils.countMessagesPendingForPlayer(secondPlayerId) == 1);

          auto firstPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(firstPlayerId);
          auto secondPlayerGameplayMessage = utils.consumeOneGameplayMessageForPlayer(secondPlayerId);

          REQUIRE(firstPlayerGameplayMessage->messageType == GAME_OVER_WIN);
          REQUIRE(secondPlayerGameplayMessage->messageType == GAME_OVER_WIN);

          auto firstPlayerGameOverWinMessage = utils.unwrapGameplayMessage<GameOverWinMessage>(firstPlayerGameplayMessage);
          auto secondPlayerGameOverWinMessage = utils.unwrapGameplayMessage<GameOverWinMessage>(secondPlayerGameplayMessage);

          REQUIRE(firstPlayerGameOverWinMessage.gameResult.winningPlayer == P1);
          REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.assignment == P1);
          REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.inputPressed.value() == expectedInput);
          REQUIRE(firstPlayerGameOverWinMessage.gameResult.player1.timeToPress.value() == timeToPress);
          REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.assignment == P2);
          REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.inputPressed.has_value() == false);
          REQUIRE(firstPlayerGameOverWinMessage.gameResult.player2.timeToPress.has_value() == false);


          REQUIRE(secondPlayerGameOverWinMessage.gameResult.winningPlayer == P1);
          REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.assignment == P1);
          REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.inputPressed.value() == expectedInput);
          REQUIRE(secondPlayerGameOverWinMessage.gameResult.player1.timeToPress.value() == timeToPress);
          REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.assignment == P2);
          REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.inputPressed.has_value() == false);
          REQUIRE(secondPlayerGameOverWinMessage.gameResult.player2.timeToPress.has_value() == false);
        }
      }
    }
  }
}

SCENARIO("Rooms - Replay", "[room][gameplay]") {
  REQUIRE(1 == 0);
}