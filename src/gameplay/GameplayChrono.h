#pragma once

#include <chrono>

using ChronoTimePoint = std::chrono::_V2::system_clock::time_point;

static const ChronoTimePoint getTimeNow() {
  return std::chrono::high_resolution_clock::now();
}

static const int getSecondsSince(ChronoTimePoint reference) {
  return std::chrono::duration_cast<std::chrono::seconds>(getTimeNow() - reference).count();
};