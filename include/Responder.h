#pragma once

#include <vector>
#include <stdint.h>

#include "SmartPortHandler.h"

class Responder {

public:
  Responder(SmartPortHandler* handler);
  std::vector<uint8_t> process(const std::vector<uint8_t>& packet);

private:
  SmartPortHandler* smartPortHandler_;
};