#pragma once

#include <vector>
#include <stdint.h>

class SPoSLIP {
public:
  uint8_t request_sequence_number_;

  uint8_t request_sequence_number() const { return request_sequence_number_; }
  void set_request_sequence_number(uint8_t request_sequence_number) { request_sequence_number_ = request_sequence_number; }

};
