#include "Server.h"

const int PORT = 8081;

void from_json() {

};

int main() {
  Server server = Server();
  server.start(PORT);
  return 0;
}