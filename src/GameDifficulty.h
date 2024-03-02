#ifndef GAMEDIFFICULTY_H
#define GAMEDIFFICULTY_H

#include <string>
#include <unordered_map>

enum GameDifficulty
{
  EASY,
  MEDIUM,
  HARD
};

static std::unordered_map<std::string, GameDifficulty> const gameDifficultyStringToEnum = {
    {"easy", GameDifficulty::EASY},
    {"medium", GameDifficulty::MEDIUM},
    {"hard", GameDifficulty::HARD},
};

#endif