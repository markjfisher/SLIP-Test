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

class TCPDevice : public BaseDevice
{
public:
  TCPDevice(const std::string &ip_address, int port) : ip_address_(ip_address), port_(port) {}

  void close() {
    ::close(writer_handle_);
  }

  void start_listener() {
#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

    // Create a TCP socket.
    listener_handle_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_handle_ < 0)
    {
      std::cerr << "Error creating TCP socket: " << std::strerror(errno) << std::endl;
      exit(1);
    }

    // Bind the socket to the specified port.
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port_);
    address.sin_addr.s_addr = inet_addr(ip_address_.c_str());
    int bind_result = bind(listener_handle_, (const sockaddr *)&address, sizeof(address));
    if (bind_result < 0)
    {
      std::cerr << "Error binding TCP socket to port: " << port_ << std::endl;
      exit(1);
    }

    // Listen for incoming connections.
    int listen_result = listen(listener_handle_, 1);
    if (listen_result < 0)
    {
      std::cerr << "Error listening for incoming connections: " << std::strerror(errno) << std::endl;
      exit(1);
    }
    std::cout << "Service running" << std::endl;

#ifdef _WIN32
    WSACleanup();
#endif
  }

  virtual void Write(const std::vector<uint8_t> &data) override
  {
    // Create a TCP socket.
#ifdef _WIN32
    listener_handle_ = WSASocket(AF_INET, SOCK_STREAM, 0);
#else
    writer_handle_ = socket(AF_INET, SOCK_STREAM, 0);
#endif
    if (writer_handle_ < 0) {
      std::cerr << "Error creating TCP socket: " << std::strerror(errno) << std::endl;
      exit(1);
    }

    // Connect to the TCP server.
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port_);
    address.sin_addr.s_addr = inet_addr(ip_address_.c_str());
#ifdef _WIN32
    int connect_result = connect(writer_handle_, (sockaddr*)&address, sizeof(address));
#else
    int connect_result = connect(writer_handle_, (const sockaddr*)&address, sizeof(address));
#endif
    if (connect_result < 0) {
      std::cerr << "Error connecting to TCP server: " << std::strerror(errno) << std::endl;
      exit(1);
    }

    // Write the data to the TCP socket.
#ifdef _WIN32
    int bytes_sent = send(writer_handle_, reinterpret_cast<const char *>(data.data()), data.size(), 0);
#else
    int bytes_sent = write(writer_handle_, data.data(), data.size());
#endif
    if (bytes_sent < 0)
    {
      std::cerr << "Error writing data to TCP socket: " << std::strerror(errno) << std::endl;
      exit(1);
    }
    ::close(writer_handle_);
  }

  virtual std::vector<uint8_t> Read() override
  {
    int client_socket_handle_ = accept(listener_handle_, NULL, NULL);
    if (client_socket_handle_ < 0) {
      std::cerr << "Error accepting incoming connection: " << std::strerror(errno) << std::endl;
      exit(1);
    }

    // Read data from the TCP socket.
    std::vector<uint8_t> buffer(1024);
#ifdef _WIN32
    int bytes_received = recv(client_socket_handle_, reinterpret_cast<char *>(buffer.data()), buffer.size(), 0);
#else
    int bytes_received = read(client_socket_handle_, buffer.data(), buffer.size());
#endif
    if (bytes_received < 0)
    {
      std::cerr << "Error reading data from TCP socket: " << std::strerror(errno) << std::endl;
      exit(1);
    }

    // Resize the buffer to the number of bytes received.
    buffer.resize(bytes_received);

    ::close(client_socket_handle_);

    // Return the received data.
    return buffer;
  }

private:
#ifdef _WIN32
  SOCKET listener_handle_;
  SOCKET writer_handle_;
#else
  int listener_handle_;
  int writer_handle_;
#endif
  std::string ip_address_;
  int port_;
};

#endif