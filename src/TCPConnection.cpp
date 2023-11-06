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
#include <string>
#include "SLIP.h"
#include "Util.h"

std::vector<uint8_t> TCPConnection::sendData(const std::vector<uint8_t>& data) {
  std::cout << "TCPConnection::sendData, sending data:" << std::endl;
  Util::hex_dump(data);

  if (data.empty()) {
    std::cerr << "TCPConnection::sendData No data was given to send" << std::endl;
    return std::vector<uint8_t>();
  }

  auto slip_data = SLIP::encode(data);
  Util::hex_dump(slip_data);

#ifdef _WIN32
  send(socket_, reinterpret_cast<const char *>(slip_data.data()), slip_data.size(), 0);
#else
  write(socket_, slip_data.data(), slip_data.size());
#endif

  // Read the Response data
  std::vector<uint8_t> completeData;
  std::vector<uint8_t> buffer(1024);

  int valread = 0;
  do {
#ifdef _WIN32
    valread = recv(socket_, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0);
#else
    valread = read(socket_, buffer.data(), buffer.size());
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
  std::cout << "TCPConnection::sendData, received response:" << std::endl;
  Util::hex_dump(completeData);

  if (matching.empty()) {
    std::cerr << "TCPConnection::sendData Server returned " << completeData.size() << " packets. Nothing matched our request_sequence_number: " << request_sequence_number << std::endl;
  }

  return matching;
}