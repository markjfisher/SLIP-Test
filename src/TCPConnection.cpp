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


std::vector<uint8_t> TCPConnection::sendData(const std::vector<uint8_t>& data) {
  if (data.empty()) {
    std::cerr << "TCPConnection::sendData No data was given to send" << std::endl;
    return std::vector<uint8_t>();
  }

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
    return std::vector<uint8_t>();
  }

  if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    std::cerr << "Connection failed" << std::endl;
    return std::vector<uint8_t>();
  }

#ifdef _WIN32
  int bytes_sent = send(sock, reinterpret_cast<const char *>(slip_data.data()), slip_data.size(), 0);
#else
  int bytes_sent = write(sock, slip_data.data(), slip_data.size());
#endif

  // GET THE RESPONSE FROM SERVER
  std::vector<uint8_t> completeData;
  std::vector<uint8_t> buffer(1024);

  int valread = 0;
  do {
#ifdef _WIN32
    valread = recv(sock, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0);
#else
    valread = read(sock, buffer.data(), buffer.size());
#endif
    if (valread > 0) {
      completeData.insert(completeData.end(), buffer.begin(), buffer.begin() + valread);
    }
  } while (valread > 0);

  // Potentially could get multiple packets if there was some weird network/reset/buffered data, so
  // only consider the first whose request sequence number matches the one we initiated (which is data[0])
  uint8_t request_sequence_number = data[0];
  std::vector<uint8_t> matching = std::vector<uint8_t>();

  if (!completeData.empty()) {
    std::vector<std::vector<uint8_t>> packets = SLIP::splitIntoPackets(completeData.data(), completeData.size());
    for (const auto& packet : packets) {
      if (!packet.empty() && packet[0] == request_sequence_number) {
        matching = packet;
        break;
      }
    }
  }

#ifdef _WIN32
  closesocket(sock);
  WSACleanup();
#else
  close(sock);
#endif

  if (matching.empty()) {
    std::cerr << "TCPConnection::sendData Server returned " << completeData.size() << " packets. Nothing matched our request_sequence_number: " << request_sequence_number << std::endl;
  }

  return matching;
}