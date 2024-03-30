#include "Serializer.h"

using json = nlohmann::json;

const std::shared_ptr<PlayerActionMessage> Serializer::deserializePlayerActionMessage(const std::string &string) {
  try {
    json jsonObj = json::parse(string);
    const PlayerActionMessageType messageType = jsonObj.at("messageType").get<PlayerActionMessageType>();

    switch (messageType) {
      case JOIN_GAME:
        return std::make_shared<JoinGamePlayerActionMessage>(deserializeJoinGamePlayerActionMessage(jsonObj));
      case SET_READY_STATE:
        return std::make_shared<SetReadyStatePlayerActionMessage>(deserializeSetReadyStatePlayerActionMessage(jsonObj));
      case NOTIFY_CAN_PLAY:
        return std::make_shared<NotifyCanPlayPlayerActionMessage>(deserializeNotifyCanPlayPlayerActionMessage(jsonObj));
      case PLAYER_INPUT:
        return std::make_shared<InputPlayerActionMessage>(deserializeInputPlayerActionMessage(jsonObj));
      case SET_REPLAY_READY_STATE:
        return std::make_shared<SetReplayReadyStatePlayerActionMessage>(deserializeSetReplayReadyStatePlayerActionMessage(jsonObj));
      case LEAVE_GAME:
        return std::make_shared<LeaveGamePlayerActionMessage>(deserializeLeaveGamePlayerActionMessage(jsonObj));
      default:
        throw DeserializeNotHandledException();
    }
  } catch (json::exception e) {
    throw DeserializeException(e);
  }
}

const JoinGamePlayerActionMessage Serializer::deserializeJoinGamePlayerActionMessage(json &jsonObj) {
  return JoinGamePlayerActionMessage{
    jsonObj.at("nickname"),
    jsonObj.at("difficulty").get<GameDifficulty>()
  };
}

const SetReadyStatePlayerActionMessage Serializer::deserializeSetReadyStatePlayerActionMessage(json &jsonObj) {
  return SetReadyStatePlayerActionMessage{
    jsonObj.at("isReady"),
  };
}

const NotifyCanPlayPlayerActionMessage Serializer::deserializeNotifyCanPlayPlayerActionMessage(json &jsonObj) {
  return NotifyCanPlayPlayerActionMessage{};
}

const InputPlayerActionMessage Serializer::deserializeInputPlayerActionMessage(json &jsonObj) {
  return InputPlayerActionMessage{
    jsonObj.at("inputPressed").get<GameplayInput>(),
    jsonObj.at("timeToPress"),
  };
}

const SetReplayReadyStatePlayerActionMessage Serializer::deserializeSetReplayReadyStatePlayerActionMessage(json &jsonObj) {
  return SetReplayReadyStatePlayerActionMessage{
    jsonObj.at("isReadyToReplay"),
  };
}

const LeaveGamePlayerActionMessage Serializer::deserializeLeaveGamePlayerActionMessage(json &jsonObj) {
  return LeaveGamePlayerActionMessage{};
}
