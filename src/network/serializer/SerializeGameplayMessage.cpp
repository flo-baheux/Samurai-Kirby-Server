#include "Serializer.h"

using json = nlohmann::json;

const std::string Serializer::serializeGameplayMessage(const GameplayMessage &message) {
  try {

    switch (message.messageType) {
      case ROOM_JOINED:
        return serialize(dynamic_cast<const RoomJoinedMessage &>(message));
      case PLAYER_JOINED_GAME:
        return serialize(dynamic_cast<const PlayerJoinedRoomMessage &>(message));
      case PLAYER_LEFT_GAME:
        return serialize(dynamic_cast<const PlayerLeftRoomMessage &>(message));
      case PLAYER_READY_STATE_CHANGED:
        return serialize(dynamic_cast<const PlayerReadyStateChangedMessage &>(message));
      case ALL_READY_GAME_STARTING:
        return serialize(dynamic_cast<const AllReadyGameStartingMessage &>(message));
      case GAME_EXPECTING_INPUT:
        return serialize(dynamic_cast<const GameExpectingInputMessage &>(message));
      case GAME_OVER_EARLY_INPUT:
        return serialize(dynamic_cast<const GameOverEarlyInputMessage &>(message));
      case GAME_OVER_WRONG_INPUT:
        return serialize(dynamic_cast<const GameOverWrongInputMessage &>(message));
      case GAME_OVER_WIN:
        return serialize(dynamic_cast<const GameOverWinMessage &>(message));
      case GAME_OVER_DRAW:
        return serialize(dynamic_cast<const GameOverDrawMessage &>(message));
      case PLAYER_REPLAY_READY_STATE_CHANGED:
        return serialize(dynamic_cast<const PlayerReplayReadyStateChangedMessage &>(message));
      case ALL_REPLAY_READY_GAME_STARTING:
        return serialize(dynamic_cast<const AllReplayReadyGameStartingMessage &>(message));
      default:
        throw SerializeNotHandledException();
    }
  } catch (json::exception &e) {
    throw SerializeException(e);
  }
};

const std::string Serializer::serialize(const RoomJoinedMessage &message) {
  const json otherPlayer = message.otherPlayer.has_value()
    ? json{
      {"nickname", message.otherPlayer.value().nickname},
      {"assignment", message.otherPlayer.value().assignment},
      {"isReady", message.otherPlayer.value().isReady},
  }
  : nullptr;

  const json jsonMessage{
    {"messageType", message.messageType},
    {"playerAssignment", message.playerAssignment},
    {"otherPlayer", otherPlayer},
  };

  return jsonMessage.dump();
}

const std::string Serializer::serialize(const PlayerJoinedRoomMessage &message) {
  const json player{
    {"nickname", message.player.nickname},
    {"assignment", message.player.assignment},
    {"isReady", message.player.isReady},
  };

  const json jsonMessage{
    {"messageType", message.messageType},
    {"player", player},
  };

  return jsonMessage.dump();
};

const std::string Serializer::serialize(const PlayerLeftRoomMessage &message) {
  const json jsonMessage{
    {"messageType", message.messageType},
    {"playerAssignment", message.playerAssignment},
  };

  return jsonMessage.dump();
};

const std::string Serializer::serialize(const PlayerReadyStateChangedMessage &message) {
  const json jsonMessage{
    {"messageType", message.messageType},
    {"playerAssignment", message.playerAssignment},
    {"isReady", message.isReady},
  };

  return jsonMessage.dump();
}

const std::string Serializer::serialize(const AllReadyGameStartingMessage &message) {
  const json jsonMessage{
    {"messageType", message.messageType},
  };

  return jsonMessage.dump();
}

const std::string Serializer::serialize(const GameExpectingInputMessage &message) {
  const json jsonMessage{
    {"messageType", message.messageType},
    {"expectedInput", message.expectedInput},
  };

  return jsonMessage.dump();
}

const std::string Serializer::serialize(const GameOverEarlyInputMessage &message) {
  const json jsonMessage{
    {"messageType", message.messageType},
    {"gameResult", serializeGameResult(message.gameResult)},
  };

  return jsonMessage.dump();
}

const std::string Serializer::serialize(const GameOverWrongInputMessage &message) {
  const json jsonMessage{
    {"messageType", message.messageType},
    {"gameResult", serializeGameResult(message.gameResult)},
  };

  return jsonMessage.dump();
}

const std::string Serializer::serialize(const GameOverWinMessage &message) {
  const json jsonMessage{
    {"messageType", message.messageType},
    {"gameResult", serializeGameResult(message.gameResult)},
  };

  return jsonMessage.dump();
}

const std::string Serializer::serialize(const GameOverDrawMessage &message) {
  const json jsonMessage{
    {"messageType", message.messageType},
    {"gameResult", serializeGameResult(message.gameResult)},
  };

  return jsonMessage.dump();
}

const std::string Serializer::serialize(const PlayerReplayReadyStateChangedMessage &message) {
  const json jsonMessage{
    {"messageType", message.messageType},
    {"playerAssignment", message.playerAssignment},
    {"isReadyToReplay", message.isReadyToReplay},
  };

  return jsonMessage.dump();
}

const std::string Serializer::serialize(const AllReplayReadyGameStartingMessage &message) {
  const json jsonMessage{
    {"messageType", message.messageType},
  };

  return jsonMessage.dump();
}

const json Serializer::serializeGameResult(GameResultDTO gameResult) {
  return json{
    {
      "winningPlayer",
      gameResult.winningPlayer.has_value()
      ? json(gameResult.winningPlayer.value())
    : json(),
    },
    {"player1", {
      {"assignment", gameResult.player1.assignment},
    {"inputPressed", gameResult.player1.inputPressed.has_value() ? json(gameResult.player1.inputPressed.value()) : json()},
    {"timeToPress", gameResult.player1.timeToPress.has_value() ? json(gameResult.player1.timeToPress.value()) : json()},
  }},
    {"player2", {
      {"assignment", gameResult.player2.assignment},
    {"inputPressed", gameResult.player2.inputPressed.has_value() ? json(gameResult.player2.inputPressed.value()) : json()},
    {"timeToPress", gameResult.player2.timeToPress.has_value() ? json(gameResult.player2.timeToPress.value()) : json()},
  }},
  };
};
