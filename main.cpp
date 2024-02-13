#include "Server.h"
// #include <chrono>
// #include <iostream>
// #include <thread>

const int PORT = 8081;

// static auto getTimerNow()
// {
//   return std::chrono::high_resolution_clock::now();
// }

// template <typename ChronoDuration>
// static auto getTimeBetween(std::chrono::_V2::system_clock::time_point start, std::chrono::_V2::system_clock::time_point end)
// {
//   return std::chrono::duration_cast<ChronoDuration>(end - start).count();
// };

int main()
{
  // std::chrono::_V2::system_clock::time_point gameStartedAt{};

  // gameStartedAt = getTimerNow();
  // std::this_thread::sleep_for(std::chrono::seconds(1));
  // std::cout << getTimeBetween<std::chrono::seconds>(gameStartedAt, getTimerNow()) << std::endl;

  Server server = Server();

  server.serverListen(PORT);

  return 0;
}
