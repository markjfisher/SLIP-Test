#ifndef TCP_DEVICE_H
#define TCP_DEVICE_H

#include <iostream>
#include <string>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <arpa/inet.h>
#include <unistd.h>

#include "BaseDevice.h"

class TCPDevice : public BaseDevice {
public:
  TCPDevice(const std::string& ip_address, int port) : ip_address_(ip_address), port_(port) {
    // Create a TCP socket.
#ifdef _WIN32
    socket_handle_ = WSASocket(AF_INET, SOCK_STREAM, 0);
#else
    socket_handle_ = socket(AF_INET, SOCK_STREAM, 0);
#endif
    if (socket_handle_ < 0) {
      std::cerr << "Error creating TCP socket: " << std::strerror(errno) << std::endl;
      exit(1);
    }

    // Connect to the TCP server.
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port_);
    address.sin_addr.s_addr = inet_addr(ip_address_.c_str());
#ifdef _WIN32
    int connect_result = connect(socket_handle_, (sockaddr*)&address, sizeof(address));
#else
    int connect_result = connect(socket_handle_, (const sockaddr*)&address, sizeof(address));
#endif
    if (connect_result < 0) {
      std::cerr << "Error connecting to TCP server: " << std::strerror(errno) << std::endl;
      exit(1);
    }
  }

  virtual void Write(const std::vector<uint8_t>& data) override {
    // Write the data to the TCP socket.
#ifdef _WIN32
    int bytes_sent = send(socket_handle_, reinterpret_cast<const char*>(data.data()), data.size(), 0);
#else
    int bytes_sent = write(socket_handle_, data.data(), data.size());
#endif
    if (bytes_sent < 0) {
      std::cerr << "Error writing data to TCP socket: " << std::strerror(errno) << std::endl;
      exit(1);
    }
  }

  virtual std::vector<uint8_t> Read() override {
    // Read data from the TCP socket.
    std::vector<uint8_t> buffer(1024);
#ifdef _WIN32
    int bytes_received = recv(socket_handle_, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0);
#else
    int bytes_received = read(socket_handle_, buffer.data(), buffer.size());
#endif
    if (bytes_received < 0) {
      std::cerr << "Error reading data from TCP socket: " << std::strerror(errno) << std::endl;
      exit(1);
    }

    // Resize the buffer to the number of bytes received.
    buffer.resize(bytes_received);

    // Return the received data.
    return buffer;
  }

private:
#ifdef _WIN32
  SOCKET socket_handle_;
#else
  int socket_handle_;
#endif
  std::string ip_address_;
  int port_;
};


#endif