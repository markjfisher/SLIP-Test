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

}

void TCPConnection::createReadChannel() {

  std::cout << "TCPConnection::createReadChannel() - creating thread" << std::endl;
  // Start a new thread to listen for incoming data
  std::thread readingThread([self = shared_from_this()]() {
    
    std::cout << "TCPConnection::createReadChannel():readingThread - In thread, socket: " << self->getSocket() << ", connected: " << self->isConnected() << std::endl;
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
        int errsv = errno;
        if (valread < 0) {
          // timeout is fine, just reloop.
          if (errno == EAGAIN || errno == EWOULDBLOCK || errno == 0) {
            continue;
          }
          // otherwise it was a genuine error.
          std::cerr << "Error in read thread for connection, errno: " << errsv << " = " << strerror(errsv) << std::endl;
          self->setIsConnected(false);
        }
        if (valread == 0) {
          // disconnected, close connection, should remove it to: TODO
          std::cout << "Connection closed" << std::endl;
          self->setIsConnected(false);
        }
        if (valread > 0) {
          completeData.insert(completeData.end(), buffer.begin(), buffer.begin() + valread);
        }
      } while (valread == 1024);

      if (!completeData.empty()) {
        std::vector<std::vector<uint8_t>> decoded_packets = SLIP::splitIntoPackets(completeData.data(), completeData.size());
        if (!decoded_packets.empty()) {
          for (const auto& packet : decoded_packets) {
            if (!packet.empty()) {
              std::cout << "putting packet in responses map for request id " << static_cast<unsigned int>(packet[0]) << std::endl;
              // Put the data in the map until it's read.
              {
                std::lock_guard<std::mutex> lock(self->responses_mutex_);
                self->responses_[packet[0]] = packet;              
              }
              self->response_cv_.notify_all();
            }
          }
        }
        completeData.clear();
      }
    }
    std::cout << "TCP Connection thread exiting." << std::endl;
  });

  std::cout << "TCPConnection::createReadChannel() - Thread created, detaching" << std::endl;

  // Detach the thread so it runs independently
  readingThread.detach();

}

  std::string TCPConnection::toString() {
    std::stringstream ss;
    ss << Connection::toString();  // Include the output of the base class
    ss << ", TCPConnection specific info: {";
    ss << "socket = " << socket_;
    ss << "}";
    return ss.str();
  }