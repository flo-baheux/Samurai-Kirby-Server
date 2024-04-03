#pragma once

#include <random>

static int getRandomValueInRange(int minInclusive, int maxInclusive) {
  std::random_device rdev;
  std::mt19937 rgen(rdev());
  std::uniform_int_distribution<int> idist{minInclusive, maxInclusive};

  return idist(rgen);
}