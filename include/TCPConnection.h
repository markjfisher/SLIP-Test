#pragma once

#include "Connection.h"
#include <string>

class TCPConnection : public Connection {
public:
  TCPConnection(const std::string& ip_address, int port);
  virtual std::vector<uint8_t> sendData(const std::vector<uint8_t>& data) override;

private:
  std::string ip_address_;
  int port_;
};