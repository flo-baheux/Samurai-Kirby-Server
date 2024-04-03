#include "Server.h"

const int PORT = 8081;

int main() {
  Server server = Server();
  server.start(PORT);
  return 0;
}