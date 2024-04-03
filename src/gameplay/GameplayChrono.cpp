#include "GameplayChrono.h"

ChronoTimePoint getTimeNow() {
  return std::chrono::high_resolution_clock::now();
}

int getSecondsSince(ChronoTimePoint reference) {
  return std::chrono::duration_cast<std::chrono::seconds>(getTimeNow() - reference).count();
};