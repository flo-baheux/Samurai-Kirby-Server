#include <iostream>
#include <algorithm>

#include "Server.h"
#include "ConnectedClient.h"

Server::Server()
    : roomsManager{new RoomsManager()}
{
  initNetwork();
};

int Server::initNetwork()
{
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    std::cerr << "Failed to initialize Winsock" << std::endl;
    return -1;
  }
#endif
  return 0;
};

int Server::initServerSocket(int port)
{
  struct sockaddr_in serverAddr;
  // Create socket
  if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
  {
    perror("Error creating socket");
#ifdef _WIN32
    std::cerr << "Winsock error " << WSAGetLastError() << std::endl;
    WSACleanup();
#endif
    return -1;
  }

#ifdef _WIN32
  u_long mode = 1; // non-zero value for non-blocking mode
  if (ioctlsocket(serverSocket, FIONBIO, &mode) != 0)
  {
    std::cerr << "Error: Failed to set socket to non-blocking mode." << std::endl;
    closesocket(serverSocket);
    WSACleanup();
    return -1;
  }
#else
  int flags = fcntl(serverSocket, F_GETFL, 0);
  if (flags == -1)
  {
    std::cerr << "Error: Failed to get socket flags." << std::endl;
    return -1;
  }
  if (fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) == -1)
  {
    std::cerr << "Error: Failed to set socket to non-blocking mode." << std::endl;
    return -1;
  }
#endif

  // Set up server address struct
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(port);

  // Bind the socket
  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
  {
    perror("Error binding socket");
#ifdef _WIN32
    std::cerr << "Winsock error " << WSAGetLastError() << std::endl;
    closesocket(serverSocket);
    WSACleanup();
#else
    close(serverSocket);
#endif
    return -1;
  }

  // Listen for incoming connections
  if (listen(serverSocket, 10) == -1)
  {
    perror("Error listening for connections");
#ifdef _WIN32
    std::cerr << "Winsock error " << WSAGetLastError() << std::endl;
    closesocket(serverSocket);
    WSACleanup();
#else
    close(serverSocket);
#endif
    return -1;
  }

  return serverSocket;
};

int Server::serverListen(int port)
{
  serverSocket = initServerSocket(port);
  if (serverSocket == -1)
    return -1;

  std::cout << "Server listening on port " << port << std::endl;

  while (true)
  {
    try
    {
      handleNewClientConnection();

      removeDisconnectedClients();
      roomsManager->deleteEmptyRooms();
    }
    catch (std::exception e)
    {
      std::cerr << e.what() << std::endl;
    }
  }

  // Close the server socket and cleanup
#ifdef _WIN32
  closesocket(serverSocket);
  WSACleanup();
#else
  close(serverSocket);
#endif
  return 0;
};

void Server::handleNewClientConnection()
{
  int clientSocket{};
  int clientAddrLen{};
  struct sockaddr_in clientAddr
  {
  };

  // Accept a connection
  clientAddrLen = sizeof(clientAddr);

  if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1)
  {
#ifdef _WIN32
    int error = WSAGetLastError();
    if (error != WSAEWOULDBLOCK)
    {
      std::cerr << "Failed to accept connection. WSA Error code " << error << std::endl;
    }
#else
    if (!(errno == EWOULDBLOCK || errno == EAGAIN))
    {
      std::cerr << "Failed to accept connection. Errno " << errno << std::endl;
    }
#endif
    return;
  }

#ifdef _WIN32
  u_long mode = 0; // blocking mode
  if (ioctlsocket(clientSocket, FIONBIO, &mode) != 0)
  {
    std::cerr << "Error: Failed to set socket to blocking mode." << std::endl;
    closesocket(clientSocket);
    WSACleanup();
    return;
  }
#endif

  ConnectedClient *client = new ConnectedClient{ClientConnectionInfo{clientSocket, clientAddr}, roomsManager};
  connectedClients.push_back(client);

  std::cout << "Connection accepted from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
};

void Server::removeDisconnectedClients()
{
  auto iterator = connectedClients.begin();
  while (iterator != connectedClients.end())
  {
    if ((*iterator)->isDisconnected())
    {
      delete *iterator;
      iterator = connectedClients.erase(iterator);
    }
    else
    {
      iterator++;
    }
  }
}