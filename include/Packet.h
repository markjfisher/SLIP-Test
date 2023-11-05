#pragma once

#include <vector>
#include <stdint.h>

class Packet {
public:
  virtual std::vector<uint8_t> serialize() const = 0;
};