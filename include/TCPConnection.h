#pragma once

#include "Connection.h"
#include <string>
#include <memory>

class TCPConnection : public Connection, public std::enable_shared_from_this<TCPConnection> {
public:
  TCPConnection(int socket) : socket_(socket) {}
  virtual void sendData(const std::vector<uint8_t>& data) override;
  virtual void createReadChannel() override;

  int getSocket() const { return socket_; }
  void setSocket(int socket) { this->socket_ = socket; }

  std::string toString() override;

private:
  int socket_;
};