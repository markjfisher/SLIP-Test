#pragma once

#include <vector>
#include <stdint.h>
#include "SPoSLIP.h"

class Response {
public:
  virtual void serialize(std::vector<uint8_t>& data) const = 0;
  virtual void deserialize(const std::vector<uint8_t>& data) = 0;

};
