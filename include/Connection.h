#pragma once

#include <vector>

class Connection {
public:
  virtual ~Connection() = default;
  virtual void sendData(const std::vector<uint8_t>& data) = 0;
};