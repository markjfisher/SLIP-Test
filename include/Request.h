#pragma once

#include <vector>
#include <stdint.h>
#include "Response.h"
#include "SPoSLIP.h"

class Request : public Packet {
public:
  Request(uint8_t request_sequence_number, uint8_t command_number, uint8_t sp_unit) {
    this->request_sequence_number_ = request_sequence_number;
    this->command_number_ = command_number;
    this->sp_unit_ = sp_unit;
  }

  virtual Response* deserialize(const std::vector<uint8_t>& data) = 0;

  uint8_t request_sequence_number_;
  uint8_t command_number_;
  uint8_t sp_unit_;
};