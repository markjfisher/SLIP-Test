#pragma once

#include <vector>
#include <stdint.h>

class Connection {
public:
  virtual ~Connection() = default;
  virtual std::vector<uint8_t> sendData(const std::vector<uint8_t>& data) = 0;
};
