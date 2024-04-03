#pragma once

#include <chrono>

using ChronoTimePoint = std::chrono::_V2::system_clock::time_point;

ChronoTimePoint getTimeNow();
int getSecondsSince(ChronoTimePoint reference);