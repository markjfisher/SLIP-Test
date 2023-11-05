#include "TCPConnection.h"

#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
  #include <ws2tcpip.h>
#else
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <unistd.h>
#endif
#include <iostream>
#include "SLIP.h"

TCPConnection::TCPConnection(const std::string& ip_address, int port)
  : ip_address_(ip_address), port_(port) {}


void TCPConnection::sendData(const std::vector<uint8_t>& data) {
  auto slip_data = SLIP::encode(data);
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "Failed to initialize winsock" << std::endl;
    return;
  }
  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
#else
  int sock = socket(AF_INET, SOCK_STREAM, 2);
#endif

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port_);
  if (inet_pton(AF_INET, ip_address_.c_str(), &(server_address.sin_addr)) <= 0) {
    std::cerr << "Invalid address/ Address not supported" << std::endl;
    return;
  }

  if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    std::cerr << "Connection failed" << std::endl;
    return;
  }

#ifdef _WIN32
  int bytes_sent = send(sock, reinterpret_cast<const char *>(slip_data.data()), slip_data.size(), 0);
  closesocket(sock);
  WSACleanup();
#else
  int bytes_sent = write(sock, slip_data.data(), slip_data.size());
  close(sock);
#endif

}