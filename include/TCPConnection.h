#pragma once

#include "Connection.h"
#include <string>

class TCPConnection : public Connection {
public:
  TCPConnection(int socket) : socket_(socket) {}
  virtual std::vector<uint8_t> sendData(const std::vector<uint8_t>& data) override;

  int getSocket() const { return socket_; }
  void setSocket(int socket) { this->socket_ = socket; }

private:
  int socket_;
};