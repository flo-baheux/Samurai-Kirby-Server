#include "Server.h"
#include "Serializer.h"

#include "GameplayChrono.h"

#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

const int PORT = 8081;

int main()
{
  Server server = Server();
  server.start(PORT);
  return 0;
}