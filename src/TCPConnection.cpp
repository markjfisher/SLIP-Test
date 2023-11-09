#include "TCPConnection.h"

#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
  #include <ws2tcpip.h>
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
#endif
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include "SLIP.h"
#include "Util.h"

void TCPConnection::sendData(const std::vector<uint8_t>& data) {
  std::cout << "TCPConnection::sendData, sending data:" << std::endl;
  Util::hex_dump(data);

  if (data.empty()) {
    std::cerr << "TCPConnection::sendData No data was given to send" << std::endl;
    return;
  }

  auto slip_data = SLIP::encode(data);
  Util::hex_dump(slip_data);

#ifdef _WIN32
  send(socket_, reinterpret_cast<const char *>(slip_data.data()), slip_data.size(), 0);
#else
  write(socket_, slip_data.data(), slip_data.size());
#endif

  // TODO: I've commented this out. The return data is now coming back to the Read Channel, so we have
  // to instead keep this request around until it times out or we get a response to it.



//   // Read the Response data
//   std::vector<uint8_t> completeData;
//   std::vector<uint8_t> buffer(1024);

//   int valread = 0;
//   do {
// #ifdef _WIN32
//     valread = recv(socket_, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0);
// #else
//     valread = read(socket_, buffer.data(), buffer.size());
// #endif
//     if (valread > 0) {
//       completeData.insert(completeData.end(), buffer.begin(), buffer.begin() + valread);
//     }
//   } while (valread > 0);

//   // Potentially could get multiple packets if there was some weird network/reset/buffered data, so
//   // only consider the first whose request sequence number matches the one we initiated (which is data[0])
//   uint8_t request_sequence_number = data[0];
//   std::vector<uint8_t> matching = std::vector<uint8_t>();

//   if (!completeData.empty()) {
//     std::vector<std::vector<uint8_t>> packets = SLIP::splitIntoPackets(completeData.data(), completeData.size());
//     for (const auto& packet : packets) {
//       if (!packet.empty() && packet[0] == request_sequence_number) {
//         matching = packet;
//         break;
//       }
//     }
//   }
//   std::cout << "TCPConnection::sendData, received response:" << std::endl;
//   Util::hex_dump(completeData);

//   if (matching.empty()) {
//     std::cerr << "TCPConnection::sendData Server returned " << completeData.size() << " packets. Nothing matched our request_sequence_number: " << request_sequence_number << std::endl;
//   }

//   return matching;
}

void TCPConnection::createReadChannel() {

  // Start a new thread to listen for incoming data
  std::thread readingThread([self = shared_from_this()]() {

    std::vector<uint8_t> completeData;
    std::vector<uint8_t> buffer(1024);

    // Set a timeout on the socket
    struct timeval timeout;
    timeout.tv_sec = 10;  // 10 second
    timeout.tv_usec = 0;
    setsockopt(self->getSocket(), SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    while (self->isConnected()) {
      int valread = 0;
      do {
#ifdef _WIN32
        valread = recv(self->getSocket(), reinterpret_cast<char*>(buffer.data()), buffer.size(), 0);
#else
        valread = read(self->getSocket(), buffer.data(), buffer.size());
#endif
        if (valread < 0) {
          // timeout is fine, just reloop.
          if (errno == EAGAIN || errno == EWOULDBLOCK) {
            continue;
          }
          // otherwise it was a genuine error.
          std::cerr << "Error in read thread for connection X" << strerror(errno) << std::endl;
          self->setIsConnected(false);
        }
        if (valread == 0) {
          // disconnected, this will usually come to us via a command
          std::cout << "Connection closed" << std::endl;
          self->setIsConnected(false);
        }
        if (valread > 0) {
          completeData.insert(completeData.end(), buffer.begin(), buffer.begin() + valread);
        }
      } while (valread > 0);

      // do something with the received data. This is data coming from the Responder (e.g. FN-PC), so is normally a response.
      // NOTE: we should keep alive commands as well as SP requests.
      // In this application, an "empty" SLIP frame will represent a heartbeat and requires an emtpy SLIP frame in response

      if (!completeData.empty()) {
        std::vector<std::vector<uint8_t>> decoded_packets = SLIP::splitIntoPackets(completeData.data(), completeData.size());
        if (!decoded_packets.empty()) {
          for (const auto& packet : decoded_packets) {
            if (packet.empty()) {
              // Heartbeat request, respond with an empty packet.
              // TODO
            } else {
              // Data originating from a Request, this is its response.
              
            }
          }
        }
      }
    }
  });

  // Detach the thread so it runs independently
  readingThread.detach();

}