#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include "GameplayRoom.h"

static bool isTimePointAtDefault(std::chrono::_V2::system_clock::time_point timePoint)
{
  return timePoint == std::chrono::system_clock::time_point::min();
}

static auto getTimerNow()
{
  return std::chrono::high_resolution_clock::now();
}

template <typename ChronoDuration>
static auto getTimeBetween(std::chrono::_V2::system_clock::time_point start, std::chrono::_V2::system_clock::time_point end)
{
  return std::chrono::duration_cast<ChronoDuration>(end - start).count();
};

static int getRandomValueInRange(int minInclusive, int maxInclusive)
{
  std::random_device rdev;
  std::mt19937 rgen(rdev());
  std::uniform_int_distribution<int> idist{minInclusive, maxInclusive};

  return idist(rgen);
}

GameplayRoom::GameplayRoom(GameDifficulty difficulty)
    : difficulty{difficulty}
{
  gameplayThread = std::thread(&GameplayRoom::handleGameplay, this);
}

bool GameplayRoom::addPlayer(ConnectedClient *player)
{
  if (isFull())
    return false;

  if (player1 == nullptr)
  {
    player1 = player;
    player1->sendMessage("player1");
    broadcastToPlayers("player1:joined-" + std::string(player1->getNickname()));
    if (player2 != nullptr)
    {
      // catchup message on join
      player1->sendMessage("player2:joined-" + std::string(player2->getNickname()));
      player1->sendMessage(std::string("player2:") + (player2->isPlayerReady() ? "ready" : "unready"));
    }
  }
  else if (player2 == nullptr)
  {
    player2 = player;
    player2->sendMessage("player2");
    broadcastToPlayers("player2:joined-" + std::string(player2->getNickname()));
    if (player1 != nullptr)
    {
      // catchup message on join
      player2->sendMessage("player1:joined-" + std::string(player1->getNickname()));
      player2->sendMessage(std::string("player1:") + (player1->isPlayerReady() ? "ready" : "unready"));
    }
  }
  return true;
};

bool GameplayRoom::isEmpty() const
{
  return player1 == nullptr && player2 == nullptr;
}

bool GameplayRoom::isFull() const
{
  return player1 != nullptr && player2 != nullptr;
}

bool GameplayRoom::isEveryoneReady() const
{
  return isFull() && player1->isPlayerReady() && player2->isPlayerReady();
}

bool GameplayRoom::everyoneCanPlay() const
{
  return isFull() && player1->playerCanPlay() && player2->playerCanPlay();
}

bool GameplayRoom::everyonePlayed() const
{
  return isFull() && !isTimePointAtDefault(player1PressedAt) && !isTimePointAtDefault(player2PressedAt);
}

bool GameplayRoom::everyoneWantsToReplay() const
{
  return isFull() && player1->playerWantsToReplay() && player2->playerWantsToReplay();
}

void GameplayRoom::handleGameplay()
{
  while (!stopThreads)
  {
    switch (gameState)
    {
    case WAITING_PLAYERS:
      handleEveryoneReady();
      break;
    case CAN_START:
      handleEveryoneCanPlay();
      break;
    case STARTED:
      handleTimeout();
      handleOnePlayerPlayedTimeout();
      handleEveryonePlayed();
      break;
    case OVER:
      handleReplay();
      break;
    }
  }
}

void GameplayRoom::playerNotifyReadyStatus(ConnectedClient *player, bool isReady)
{
  if (gameState == WAITING_PLAYERS)
    broadcastToPlayers(std::string("player") + (player == player1 ? "1" : "2") + ":" + (isReady ? "ready" : "unready"));
}

void GameplayRoom::playerNotifyWantsToReplay(ConnectedClient *player)
{
  if (gameState == OVER)
  {
    broadcastToPlayers(std::string("player") + (player == player1 ? "1" : "2") + ":replay");
  }
}

void GameplayRoom::playerNotifyDisconnect(ConnectedClient *player)
{
  gameState = WAITING_PLAYERS;
  if (player == player1)
  {
    player1 = nullptr;
    broadcastToPlayers("player1:disconnect");
  }
  else if (player == player2)
  {
    player2 = nullptr;
    broadcastToPlayers("player2:disconnect");
  }
}

// It is important to not determine who wins here
// second player to notify could be the winner and notify later because of network latency
void GameplayRoom::playerNotifyPressedButton(ConnectedClient *player)
{

  // FIXME: This method makes no sense bro
  if (player == player1)
  {
    player1PressedAt = getTimerNow();
  }
  else if (player == player2)
  {
    player2PressedAt = getTimerNow();
  }

  if ((gameState == CAN_START || gameState == STARTED) && (buttonToPress == '\0' || player->getButtonPressed() != buttonToPress))
  {
    gameState = OVER;
    if (player == player1)
    {
      player1PressedAt = getTimerNow();
      std::string messageToSend =std::string("player1:fail-");
      messageToSend += player->getButtonPressed();
      broadcastToPlayers(messageToSend);
    }
    else if (player == player2)
    {
      player2PressedAt = getTimerNow();
      std::string messageToSend =std::string("player2:fail-");
      messageToSend += player->getButtonPressed();
      broadcastToPlayers(messageToSend);
    }
  }
}

void GameplayRoom::broadcastToPlayers(const std::string &message)
{
  if (player1 != nullptr)
    player1->sendMessage(message);

  if (player2 != nullptr)
    player2->sendMessage(message);
}

char GameplayRoom::getRandomButtonToPress() const
{
  std::vector<char> letters = {'A', 'B', 'X', 'Y'};
  switch (difficulty)
  {
  default:
  case EASY:
    return letters[0]; // A
  case MEDIUM:
    return letters[getRandomValueInRange(0, 1)]; // A or B
  case HARD:
    return letters[getRandomValueInRange(0, letters.size() - 1)]; // A or B or X or Y
  }
}

void GameplayRoom::handleEveryoneReady()
{
  if (gameState == WAITING_PLAYERS && isEveryoneReady())
  {
    gameState = CAN_START;
    broadcastToPlayers("gameCanStart");
  }
}

void GameplayRoom::handleEveryoneCanPlay()
{
  if (gameState == CAN_START && everyoneCanPlay())
  {
    // FIXME: Maybe a sleep isn't the right approach and a timer would be better.
    std::this_thread::sleep_for(std::chrono::seconds(getRandomValueInRange(5, 10)));
    // Check if after thread sleep gamestate has changed (a player can disconnect)
    if (gameState != CAN_START)
      return;
    gameState = STARTED;
    gameStartedAt = getTimerNow();
    buttonToPress = getRandomButtonToPress();
    std::string messageToSend = std::string("startGame-");
    messageToSend.push_back(buttonToPress);
    broadcastToPlayers(messageToSend);
  }
}

void GameplayRoom::handleTimeout()
{
  if (gameState == STARTED && isTimePointAtDefault(player1PressedAt) && isTimePointAtDefault(player2PressedAt))
  {
    int timeSinceGameStarted = getTimeBetween<std::chrono::seconds>(gameStartedAt, getTimerNow());
    if (timeSinceGameStarted >= NO_INPUT_TIMEOUT_S)
    {

      broadcastToPlayers("draw");
      gameState = OVER;
    }
  }
}

void GameplayRoom::handleOnePlayerPlayedTimeout()
{
  if (gameState == STARTED && !everyonePlayed())
  {
    if (!isTimePointAtDefault(player1PressedAt) && isTimePointAtDefault(player2PressedAt))
    {
      int timeSincePlayerPlayed = getTimeBetween<std::chrono::seconds>(player1PressedAt, getTimerNow());
      if (timeSincePlayerPlayed >= ONE_PLAYER_ONLY_TIMEOUT_S)
      {
        broadcastToPlayers("player1:win-" + std::to_string(player1->getTimer()));
        gameState = OVER;
      }
    }

    if (isTimePointAtDefault(player1PressedAt) && !isTimePointAtDefault(player2PressedAt))
    {
      int timeSincePlayerPlayed = getTimeBetween<std::chrono::seconds>(player2PressedAt, getTimerNow());

      if (timeSincePlayerPlayed >= ONE_PLAYER_ONLY_TIMEOUT_S)
      {
        broadcastToPlayers("player2:win-" + std::to_string(player2->getTimer()));
        gameState = OVER;
      }
    }
  }
}

void GameplayRoom::handleEveryonePlayed()
{
  if (gameState == STARTED && everyonePlayed())
  {
    int player1Timer = player1->getTimer();
    int player2Timer = player2->getTimer();
    if (player1Timer < player2Timer && player1->getButtonPressed() == buttonToPress)
    {
      broadcastToPlayers("player1:win-" + std::to_string(player1Timer));
    }
    else if (player1Timer > player2Timer && player2->getButtonPressed() == buttonToPress)
    {
      broadcastToPlayers("player2:win-" + std::to_string(player2Timer));
    }
    else
    {
      broadcastToPlayers("draw:");
    }

    gameState = OVER;
  }
}

void GameplayRoom::handleReplay()
{
  if (gameState == OVER && everyoneWantsToReplay())
  {
    gameState = CAN_START;
    buttonToPress = '\0';
    player1->resetGameData();
    player2->resetGameData();
    gameStartedAt = std::chrono::system_clock::time_point::min();
    player1PressedAt = std::chrono::system_clock::time_point::min();
    player2PressedAt = std::chrono::system_clock::time_point::min();
    broadcastToPlayers("replay");
  }
}

GameDifficulty GameplayRoom::getRoomDifficulty() const
{
  return difficulty;
}

GameplayRoom::~GameplayRoom()
{
  stopThreads = true;
  if (gameplayThread.joinable())
    gameplayThread.join();
}
